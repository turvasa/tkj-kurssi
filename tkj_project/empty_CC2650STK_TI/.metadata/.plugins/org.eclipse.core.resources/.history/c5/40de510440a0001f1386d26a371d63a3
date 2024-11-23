/* C Standard library */
#include <stdio.h>

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
#include <ti/drivers/i2c/I2CCC26XX.h>

/* Board Header files */
#include "Board.h"
#include "sensors/mpu9250.h"
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>


static PIN_Handle buttonHandle;
static PIN_State buttonState;

static PIN_Handle ledHandle;
static PIN_State ledState;

static PIN_Handle hMpuPin;
static PIN_State MpuPinState;

static PIN_Handle sensorHandle;
static PIN_State sensorState;



//Sensor values array
static float GYRO[30][6];


/* Task */
#define STACKSIZE 2048



static PIN_Config MpuPinConfig[] = {
   Board_MPU_POWER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE
};

static const I2CCC26XX_I2CPinCfg i2cMPUCfg = {
   .pinSDA = Board_I2C0_SDA1,
   .pinSCL = Board_I2C0_SCL1,
};

Char sensorTaskStack[STACKSIZE];
Char uartTaskStack[STACKSIZE];


enum state { WAITING=1, DATA_READY };
enum state programState = WAITING;




PIN_Config buttonConfig[] = {
   Board_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   Board_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
   PIN_TERMINATE
};


PIN_Config ledConfig[] = {
   Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
   PIN_TERMINATE // Asetustaulukko lopetetaan aina tällä vakiolla
};

void buttonFxn(PIN_Handle handle, PIN_Id pinId) {
    uint_t pinValue = PIN_getOutputValue( Board_LED1 );
    pinValue = !pinValue;
    PIN_setOutputValue( ledHandle, Board_LED1, pinValue );

}


Void uartTaskFxn(UArg arg0, UArg arg1) {

    
       UART_Handle uart;
       UART_Params uartParams;

       // Alustetaan sarjaliikenne
       UART_Params_init(&uartParams);
       uartParams.writeDataMode = UART_DATA_TEXT;
       uartParams.readDataMode = UART_DATA_TEXT;
       uartParams.readEcho = UART_ECHO_OFF;
       uartParams.readMode=UART_MODE_BLOCKING;
       uartParams.baudRate = 9600; // nopeus 9600baud
       uartParams.dataLength = UART_LEN_8; // 8
       uartParams.parityType = UART_PAR_NONE; // n
       uartParams.stopBits = UART_STOP_ONE; // 1

       // Avataan yhteys laitteen sarjaporttiin vakiossa Board_UART0
       uart = UART_open(Board_UART0, &uartParams);
       if (uart == NULL) {
          System_abort("Error opening the UART");
       }

    while (1) {



        Task_sleep(100000 / Clock_tickPeriod);
    }
}

Void sensorTaskFxn(UArg arg0, UArg arg1) {

    float ax, ay, az, gx, gy, gz;
    I2C_Handle      i2cMPU;
    I2C_Params      i2cMPUParams;

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

    int i = 0;


    while (1) {
        mpu9250_get_data(&i2cMPU, &ax, &ay, &az, &gx, &gy, &gz);

        GYRO[i][0] = ax;
        GYRO[i][1] = ay;
        GYRO[i][2] = az;
        GYRO[i][3] = gx;
        GYRO[i][4] = gy;
        GYRO[i][5] = gz;

        char print_str[100];

        sprintf(print_str,
           "acc_x: %.2f, acc_y: %.2f, acc_z: %.2f \n"
           "gyro_x: %.2f, gyro_y: %.2f, gyro_z: %.2f\n\n",
           ax, ay, az,
           gx, gy, gz
        );

        System_printf(print_str);
        System_flush();
        //programState = DATA_READY;

        /*
        if (buttonState){
            break;
        }
        */

        i++;

        if (i % 30 == 0) {
            i = 0;
        }

        Task_sleep(100000 / Clock_tickPeriod);
    }

    I2C_close(i2cMPU);
}

/*
Void states() {
    int state = 0;
    
    while (1) {
        if


    }
}
*/


Int main(void) {

    
    Task_Handle sensorTaskHandle;
    Task_Params sensorTaskParams;
    Task_Handle uartTaskHandle;
    Task_Params uartTaskParams;

    
    Board_initGeneral();
    
    Board_initI2C();
    Board_initUART();
    
    ledHandle = PIN_open(&ledState, ledConfig);
    if(!ledHandle) {
       System_abort("Error initializing LED pin\n");
    }

   
    buttonHandle = PIN_open(&buttonState, buttonConfig);
    if(!buttonHandle) {
       System_abort("Error initializing button pin\n");
    }

    
    if (PIN_registerIntCb(buttonHandle, &buttonFxn) != 0) {
       System_abort("Error registering button callback function");
    }

   sensorHandle = PIN_open(&MpuPinState, MpuPinConfig);
   if(!sensorHandle) {
       System_abort("Error initializing sensor pin\n");
    }
    

    /* Task */
    Task_Params_init(&sensorTaskParams);
    sensorTaskParams.stackSize = STACKSIZE;
    sensorTaskParams.stack = &sensorTaskStack;
    sensorTaskParams.priority=2;
    sensorTaskHandle = Task_create(sensorTaskFxn, &sensorTaskParams, NULL);
    if (sensorTaskHandle == NULL) {
        System_abort("Task create failed!");
    }

    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = STACKSIZE;
    uartTaskParams.stack = &uartTaskStack;
    uartTaskParams.priority=2;
    uartTaskHandle = Task_create(uartTaskFxn, &uartTaskParams, NULL);
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
