#include <stdio.h>
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

#include "Board.h"
#include "sensors/mpu9250.h"

#define STACKSIZE 2048
#define MORSE_UNIT_DURATION 50000  // Duration of a Morse unit in microseconds

Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];

// State Machine Enum
enum state { WAITING, RECEIVING, PLAYING };
enum state programState = WAITING;

static PIN_Handle buttonHandle, ledHandle, hMpuPin;
static PIN_State buttonState, ledState, MpuPinState;

PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE
};

PIN_Config ledConfig[] = {
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

static PIN_Config MpuPinConfig[] = {
   Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
   .pinSDA = Board_I2C0_SDA1,
   .pinSCL = Board_I2C0_SCL1,
};

// Morse Code Table
const char* morse_code[26] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--.."
};

// Function Prototypes
void play_morse_with_led(const char *message);
void interpret_morse(float ax, float ay, float az);

static int spaceCount = 0;
char currentWord[100] = "";
int wordIndex = 0;

void buttonFxn(PIN_Handle handle, PIN_Id pinId) {
    uint_t pinValue = PIN_getInputValue(pinId);

    if (pinValue == 0) {
        spaceCount++;


        currentWord[wordIndex++] = ' ';
        System_printf(" ");
        System_flush();


        if (spaceCount == 2) {
            currentWord[wordIndex] = '\0';
            System_printf("word completed: %s\n", currentWord);
            System_flush();

            translate_morse_to_text(currentWord);
            programState = PLAYING;

            wordIndex = 0;
            spaceCount = 0;

        }


        Task_sleep(100 / Clock_tickPeriod);
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

Void uartTaskFxn(UArg arg0, UArg arg1) {
    UART_Handle uart;
    UART_Params uartParams;
    char input[100];
    int len;

    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readEcho = UART_ECHO_ON;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.baudRate = 9600;

    uart = UART_open(Board_UART0, &uartParams);
    if (uart == NULL) {
       System_abort("Error opening the UART");
    }

    while (1) {
        if(programState == PLAYING) {
        System_printf("Enter word:\n");
        System_flush();

        len = UART_read(uart, input, sizeof(input) - 1);
        if (len > 0) {
            play_morse_with_led(input);
            }
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

    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
       System_abort("Error Initializing I2C\n");
    }
    Task_sleep(10000 / Clock_tickPeriod);
    mpu9250_setup(&i2cMPU);

    while (1) {
        if(programState == WAITING) {
        mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
        interpret_morse(ax, ay, az);
        }

        if (spaceCount == 2 && programState == RECEIVING) {
            programState = PLAYING;
            System_printf("Word: %s\n", currentWord);
            System_flush();
            spaceCount = 0;
            currentWord[0] = '\0';
        }

        Task_sleep(100000 / Clock_tickPeriod);
    }

    I2C_close(i2cMPU);
}

void interpret_morse(float ax, float ay, float az) {



    if (ax > 0.5) {
        currentWord[wordIndex++] = '.';
        System_printf(".\n");
        System_flush();
    } else if (ay > 0.5) {
        currentWord[wordIndex++] = '-';
        System_printf("-\n");
        System_flush();
    }




    if (spaceCount == 2) {
        currentWord[wordIndex] = '\0';
        System_printf("Word completed: %s\n", currentWord);
        System_flush();

        translate_morse_to_text(currentWord);
        wordIndex = 0;
        spaceCount = 0;

    }
}


void play_morse_with_led(const char *message) {
    int i;
    System_printf("Playing code with LED");
    System_flush();

    for(i = 0; message[i] != '\0'; i++) {
        char ch = message[i];

        if(ch == '.') {
            PIN_setOutputValue(ledHandle, Board_LED1, 1);
            Task_sleep(MORSE_UNIT_DURATION / Clock_tickPeriod);
            PIN_setOutputValue(ledHandle, Board_LED1, 0);
            Task_sleep(MORSE_UNIT_DURATION / Clock_tickPeriod);
        } else if(ch == '-') {
            PIN_setOutputValue(ledHandle, Board_LED1, 1);
            Task_sleep(3 * MORSE_UNIT_DURATION / Clock_tickPeriod);
            PIN_setOutputValue(ledHandle, Board_LED1, 0);
            Task_sleep(MORSE_UNIT_DURATION / Clock_tickPeriod);
        } else if(ch == ' ') {
            Task_sleep(3 * MORSE_UNIT_DURATION / Clock_tickPeriod);
        }
    }
    System_printf("Finished playing code with LED");
    System_flush();
}

Int main(void) {
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    Board_initGeneral();
    Board_initI2C();
    Board_initUART();

    ledHandle = PIN_open(&ledState, ledConfig);
    if (!ledHandle) {
       System_abort("Error initializing LED pin\n");
    }

    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if (!buttonHandle) {
       System_abort("Error initializing button pin\n");
    }

    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
       System_abort("Error registering button callback function");
    }

    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (!hMpuPin) {
       System_abort("Error initializing sensor pin\n");
    }

    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = STACKSIZE;
    sensorTaskParams.stack = &sensorTaskStack;
    sensorTaskParams.priority = 1;
    sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
    if (sensorTaskHandle == NULL) {
        System_abort("Error creating sensor task\n");
    }

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskParams.priority = 1;
    uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
    if (uartTaskHandle == NULL) {
        System_abort("Error creating UART task\n");
    }

    BIOS_start();  // Start BIOS
}
