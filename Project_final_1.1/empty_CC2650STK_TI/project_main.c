#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/i2c/I2CCC26XX.h>
#include <ctype.h>
#include <buzzer.h>
#include "Board.h"
#include "sensors/mpu9250.h"


//Stack size
#define STACKSIZE 2048

//Define stacks
Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];
Char buzzerTaskStack[STACKSIZE];


//Program states
enum state { WAITING = 1, RECEIVING, SENDING, PLAYING };
enum state programState = WAITING;


//Configure pins
static PIN_Handle buttonHandle, ledHandle, hMpuPin, hBuzzer;
static PIN_State buttonState, ledState, MpuPinState, sBuzzer;

//Configure of pins
PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

//Configure of buttons
PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE
};

//Configure of led
PIN_Config ledConfig[] = {
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

//Configure of acceleration / gyro sensor (MPU)
static PIN_Config MpuPinConfig[] = {
   Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
   .pinSDA = Board_I2C0_SDA1,
   .pinSCL = Board_I2C0_SCL1,
};


// Morse Code Table (In alphapetical order)
const char* morse_code[26] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--.."
};


//Global variables
char currentWord[100] = "";
int wordIndex = 0;
int counter = 0;
char uartBuffer = 'x';
char message[4];
uint8_t buffer[32];




//Change program state, when button is pressed depending on current state
void buttonFxn(PIN_Handle handle, PIN_Id pinId) {
    if(programState == WAITING) {
        programState = RECEIVING;
        PIN_setOutputValue(ledHandle, Board_LED1, 1);

    } else if (programState == RECEIVING) {
        programState = WAITING;
        PIN_setOutputValue(ledHandle, Board_LED1, 0);
    }
}



void playBuzzer(int frequency, int time) {
    buzzerOpen(hBuzzer);
    buzzerSetFrequency(frequency);
    Task_sleep(time / Clock_tickPeriod);
    buzzerClose();
}



void setMessage(char morse) {
    sprintf(message, "morse\r\n\0");
    programState = SENDING;
    Task_sleep(650000 / Clock_tickPeriod);
}



//Recognize movement and 
void interpret_morse(float ax, float ay, float az) {

    //Movement to point ('.')
    if (ax > 0.3) {
        playBuzzer(4000, 120000);
        setMessage('.');
    } 
    
    //Movement to line ('-')
    else if (ay > 0.3) {
        playBuzzer(4000, 300000);
        setMessage('-');
    } 
    
    //Movement to space (' ')
    else if(az > 0.3) {
        setMessage(' ');
    }
}



void translate_morse_to_text(char *morseWord) {
    char translatedWord[100] = "";
    int i = 0, j = 0;

    while (morseWord[i] != '\0') {
        char morseChar[10];
        int k = 0;

        while (morseWord[i] != ' ' && morseWord[i] != '\0') {//Iterate until we reach 0. Each - . is added to morseChar
            morseChar[k++] = morseWord[i++];
        }
        morseChar[k] = '\0'; //Add null to the end

        int idx;
        for (idx = 0; idx < 26; idx++) {
            if (strcmp(morseChar, morse_code[idx]) == 0) { //strcmp compares morseChar to morse_code at idx. If match is found it returns 0.
                translatedWord[j++] = 'A' + idx;  //gets ASCII value of letter since A is first we add by the index.
                break;
            }
        }

        if (morseWord[i] == ' ') {
            i++;
        }
    }

    translatedWord[j] = '\0';  // Null-terminate the translated word
    System_printf("%s\n", translatedWord);
    System_flush();
}



void uartFxn(UART_Handle uart, void *buf, size_t len) {

    if (strcmp(buf, "-") == 0) {
        uartBuffer = '-';
        programState = PLAYING;
    }
    
    if (strcmp(buf, ".") == 0) {
        uartBuffer = '.';
        programState = PLAYING;
    }

    UART_read(uart, buf, 1);
}



void uartTaskFxn(UArg arg0, UArg arg1) {
    UART_Handle uart;
    UART_Params uartParams;

    UART_Params_init(&uartParams);
        uartParams.writeDataMode = UART_DATA_TEXT;
        uartParams.readDataMode = UART_DATA_TEXT;
        uartParams.readEcho = UART_ECHO_OFF;
        uartParams.readMode=UART_MODE_CALLBACK;
        uartParams.readCallback = &uartFxn;
        uartParams.baudRate = 9600;
        uartParams.dataLength = UART_LEN_8;
        uartParams.parityType = UART_PAR_NONE;
        uartParams.stopBits = UART_STOP_ONE;

    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
       System_abort("Error opening the UART");
    }

    UART_read(uart, buffer, 1);

    while (1) {
        if(programState == SENDING) {
            //Send data to Serial Client
            UART_write(uart, message, 4);
            programState = RECEIVING;
        }
        Task_sleep(100000 / Clock_tickPeriod);
    }
}



Void sensorTaskFxn(UArg arg0, UArg arg1) {
    float ax, ay, az, gx, gy, gz;
    I2C_Handle i2cMPU;
    I2C_Params i2cMPUParams;

    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;

    PIN_setOutputValue(hMpuPin, Board_MPU_POWER, Board_MPU_POWER_ON);
    Task_sleep(100000 / Clock_tickPeriod);
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
       System_abort("Error Initializing I2C\n");
    }

    mpu9250_setup(&i2cMPU);

    while (1) {
        //Get data from MPU 9250
        if (programState == RECEIVING) {
            mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
            interpret_morse(ax, ay, az);
        }

        Task_sleep(100000 / Clock_tickPeriod);
    }
}



void buzzerTaskFxn(const char *message) {
    while(1) {

        //Don't do anything if program state is not PLAYING
        if(programState != PLAYING) {
            Task_sleep(62000 / Clock_tickPeriod);
            continue;
        }

        //Given char is point ('.')
        if (uartBuffer == '.') {
            playBuzzer(4000, 120000);
        } 
        
        //Given char is line ('-')
        else if (uartBuffer == '-') {
            playBuzzer(4000, 300000);
        } 
        
        //Given char is space (' ')
        else if (uartBuffer == ' ') {
            Task_sleep(120000/ Clock_tickPeriod);
        }

        uartBuffer = 'x';
        programState = WAITING;
        Task_sleep(62000 / Clock_tickPeriod);
    }
 }



Int main(void) {
    //Sensor
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;

    //UART
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    //Buzzer
    Task_Handle buzzerTask;
    Task_Params buzzerTaskParams;

    Board_initGeneral();
    Board_initI2C();
    Board_initUART();

    //Led hadle
    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) {
       System_abort("Error initializing LED pin\n");
    }

    //Button hadle
    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if (!buttonHandle) {
       System_abort("Error initializing button pin\n");
    }

    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
       System_abort("Error registering button callback function");
    }

    //MPU handle
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (!hMpuPin) {
       System_abort("Error initializing sensor pin\n");
    }

    //Buzzer handle
    hBuzzer = PIN_open(&sBuzzer, cBuzzer);
    if(hBuzzer == NULL) {
        System_abort("Pin open failed!");
    }

    //Sensor task
    Task_Params_init(&sensorTaskParams);
        sensorTaskParams.stackSize = STACKSIZE;
        sensorTaskParams.stack = &sensorTaskStack;
        sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
        if (sensorTaskHandle == NULL) {
            System_abort("Error creating sensor task\n");
        }

    //UART task
    Task_Params_init(&uartTaskParams);
        uartTaskParams.stackSize = STACKSIZE;
        uartTaskParams.stack = &uartTaskStack;
        uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
        if (uartTaskHandle == NULL) {
            System_abort("Error creating UART task\n");
        }

    //Buzzer task
    Task_Params_init(&buzzerTaskParams);
        buzzerTaskParams.stackSize = STACKSIZE;
        buzzerTaskParams.stack = &buzzerTaskStack;
        buzzerTask = Task_create((Task_FuncPtr)buzzerTaskFxn, &buzzerTaskParams, NULL);
        if (buzzerTask == NULL) {
            System_abort("Task create failed!");
        }

    BIOS_start();  // Start BIOS
    return (0);

}
