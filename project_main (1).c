/* C Standard library */
#include <buzzer.h>
#include <stdio.h>
#include <string.h>

/* XDCtools files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Board.h"
#include <ti/drivers/i2c/I2CCC26XX.h>
#include "sensors/mpu9250.h"

/* Task */
#define STACKSIZE 2048
Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];
Char taskStack[STACKSIZE];
Char buzzerTaskStack[STACKSIZE];

// Declaration of finite state machine
enum state {RECIEVE_DATA = 1, RECORD_DATA, SEND_DATA, PLAYING_DATA};
enum state programState = RECIEVE_DATA;

//Global variables
float ax, ay, az, gx, gy, gz;
char charData[4];
char inputData = '!';
uint8_t uartBuffer[32];

// MPU power pin global variables
static PIN_Handle hMpuPin;
static PIN_State  MpuPinState;
static PIN_Handle buttonHandle;
static PIN_State buttonState;
static PIN_Handle ledHandle;
static PIN_State ledState;
static PIN_Handle hBuzzer;
static PIN_State sBuzzer;


// MPU power pin
static PIN_Config MpuPinConfig[] = {
    Board_MPU_POWER  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config ledConfig[] = {
   Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE
};

PIN_Config cBuzzer[] = {
  Board_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
  PIN_TERMINATE
};

// MPU uses its own I2C interface
static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
    .pinSDA = Board_I2C0_SDA1,
    .pinSCL = Board_I2C0_SCL1
};

//int strcmp (const char* str1, const char* str2);


void checkMove() {
    if (gx > 225) {
        PIN_setOutputValue( ledHandle, Board_LED0, 1);
        sprintf(charData, " \r\n\0");
        programState = SEND_DATA;
        Task_sleep(700000 / Clock_tickPeriod);
        PIN_setOutputValue( ledHandle, Board_LED0, 0);
    }
    else if (az < 0.2 && ay < 0.8 && ax < 0.8 && ay > -0.8) {
        PIN_setOutputValue( ledHandle, Board_LED0, 1);
        sprintf(charData, ".\r\n\0");
        programState = SEND_DATA;
        Task_sleep(700000 / Clock_tickPeriod);
        PIN_setOutputValue( ledHandle, Board_LED0, 0);
    }
    else if (ay < -0.5 && az > 0.8) {
        PIN_setOutputValue( ledHandle, Board_LED0, 1);
        sprintf(charData, "-\r\n\0");
        programState = SEND_DATA;
        Task_sleep(700000 / Clock_tickPeriod);
        PIN_setOutputValue( ledHandle, Board_LED0, 0);
    }
}


void buttonFxn(PIN_Handle handle, PIN_Id pinId) {

    if (programState == RECIEVE_DATA) {
        PIN_setOutputValue( ledHandle, Board_LED1, 1 );
        programState = RECORD_DATA;
    }
    else if(programState == RECORD_DATA) {
        PIN_setOutputValue( ledHandle, Board_LED1, 0 );
        programState = RECIEVE_DATA;
    }

}

void uartFxn(UART_Handle handle, void *rxBuf, size_t len) {

    if(strcmp(rxBuf, ".") == 0) {
        inputData = '.';
        programState = PLAYING_DATA;
    }
    else if(strcmp(rxBuf, "-") == 0) {
        inputData = '-';
        programState = PLAYING_DATA;
    }
    UART_read(handle, rxBuf, 1);
}

/* Task Functions */
Void uartTaskFxn(UArg arg0, UArg arg1) {

    // UART-kirjaston asetukset
    UART_Handle uartHandle;
    UART_Params uartParams;

    // Alustetaan sarjaliikenne
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readMode = UART_MODE_CALLBACK;
    uartParams.readCallback = &uartFxn;
    uartParams.baudRate = 9600; // nopeus 9600baud
    uartParams.dataLength = UART_LEN_8; // 8
    uartParams.parityType = UART_PAR_NONE; // n
    uartParams.stopBits = UART_STOP_ONE; // 1

    // Avataan yhteys laitteen sarjaporttiin vakiossa Board_UART0
    uartHandle = UART_open(Board_UART0, &uartParams);
    if (uartHandle == NULL) {
       System_abort("Error opening the UART");
    }

    UART_read(uartHandle, uartBuffer, 1);

    while (1) {

        if(programState == SEND_DATA) {

            UART_write(uartHandle, charData, 4);
            programState = RECORD_DATA;
        }

//        else if (programState == RECIEVE_DATA){
//            UART_read(uart, &inputData, 1);
//            if(inputData != '!') {
//                programState = PLAYING_DATA;
//            }
//        }

        Task_sleep(100000 / Clock_tickPeriod);

    }
}

Void buzzerTaskFxn(UArg arg0, UArg arg1) {

  while (1) {
    if(programState == PLAYING_DATA) {
        if(inputData == '.') {
            buzzerOpen(hBuzzer);
            buzzerSetFrequency(4000);
            Task_sleep(50000 / Clock_tickPeriod);
            buzzerClose();
        }
        else if(inputData == '-') {
            buzzerOpen(hBuzzer);
            buzzerSetFrequency(4000);
            Task_sleep(400000 / Clock_tickPeriod);
            buzzerClose();
        }
        inputData = '!';
        programState = RECIEVE_DATA;
    }
    Task_sleep(50000 / Clock_tickPeriod);
  }

}

Void sensorTaskFxn(UArg arg0, UArg arg1) {

    I2C_Handle i2cMPU; // Own i2c-interface for MPU9250 sensor
    I2C_Params i2cMPUParams;

    I2C_Params_init(&i2cMPUParams);
    i2cMPUParams.bitRate = I2C_400kHz;
    // Note the different configuration below
    i2cMPUParams.custom = (uintptr_t)&i2cMPUCfg;

    // MPU power on
    PIN_setOutputValue(hMpuPin,Board_MPU_POWER, Board_MPU_POWER_ON);

    // Wait 100ms for the MPU sensor to power up
    Task_sleep(100000 / Clock_tickPeriod);
    System_printf("MPU9250: Power ON\n");
    System_flush();

    // MPU open i2c
    i2cMPU = I2C_open(Board_I2C, &i2cMPUParams);
    if (i2cMPU == NULL) {
        System_abort("Error Initializing I2CMPU\n");
    }

    // MPU setup and calibration
    System_printf("MPU9250: Setup and calibration...\n");
    System_flush();

    mpu9250_setup(&i2cMPU);

    System_printf("MPU9250: Setup and calibration OK\n");
    System_flush();

    // Loop forever
    while (1) {

        if(programState == RECORD_DATA) {
            // MPU ask data
            mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);
            checkMove();
        }
        Task_sleep(100000 / Clock_tickPeriod);
    }
}

Int main(void) {

    // Task variables
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    Task_Handle buzzerTask;
    Task_Params buzzerTaskParams;

    // Initialize board
    Board_initGeneral();
    Board_initI2C();

    // Otetaan sarjaportti käyttöön ohjelmassa
    Board_initUART();

    // Open MPU power pin
    hMpuPin = PIN_open(&MpuPinState, MpuPinConfig);
    if (hMpuPin == NULL) {
    	System_abort("Pin open failed!");
    }

    // Ledi käyttöön ohjelmassa
    ledHandle = PIN_open( &ledState, ledConfig );
    if(!ledHandle) {
       System_abort("Error initializing LED pin\n");
    }

    // Painonappi käyttöön ohjelmassa
    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if(!buttonHandle) {
       System_abort("Error initializing button pin\n");
    }

    // Painonapille keskeytyksen käsittellijä
    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
       System_abort("Error registering button callback function");
    }

    // Buzzer
     hBuzzer = PIN_open(&sBuzzer, cBuzzer);
     if (hBuzzer == NULL) {
       System_abort("Pin open failed!");
     }

     Task_Params_init(&buzzerTaskParams);
     buzzerTaskParams.stackSize = STACKSIZE;
     buzzerTaskParams.stack = &buzzerTaskStack;
     buzzerTask = Task_create((Task_FuncPtr)buzzerTaskFxn, &buzzerTaskParams, NULL);
     if (buzzerTask == NULL) {
       System_abort("Task create failed!");
     }


    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = STACKSIZE;
    sensorTaskParams.stack = &sensorTaskStack;
    sensorTaskHandle = Task_create((Task_FuncPtr)sensorTaskFxn, &sensorTaskParams, NULL);
    if (sensorTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskHandle = Task_create((Task_FuncPtr)uartTaskFxn, &uartTaskParams, NULL);
    if (uartTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    /* Sanity check */
    System_printf("Hello world!\n");
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
