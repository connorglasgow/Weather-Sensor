//Connor Glasgow
//1001322122

//----------------------------------------
//  Includes, etc.
//----------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "adc0.h"
#include "wait.h"
#include "gpio.h"
#include "commandline.h"

//define pinouts
#define COMPARATOR PORTC,7
#define MEAS_LR PORTA,7
#define MEAS_C  PORTA,6
#define HIGHSIDE_R PORTA,5
#define LOWSIDE_R PORTA,2
#define INTEGRATE PORTA,3

//analog input for ADC stuff
#define AIN1 PORTE,2

//defines for commandline interface
#define MAX_CHARS 80
#define MAX_FIELDS 5

//'state machine' for interfacing with which measurement is taken
//also possibly for AUTO command
typedef enum _STATE
{
    resistance, capacitance, inductance, none
} STATE;

//----------------------------------------
// TEMP GLOBAL VARS
//----------------------------------------
//for time values for measuring components
uint32_t time = 0;
//for commandline interface
extern bool enterPressed;
STATE state;
//for sprintf stuff
char outstr[100];

//----------------------------------------
//  Functions
//----------------------------------------

void initHW()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    //init I2C
    initI2c0();

    // Enable clocks
    //enablePort(?????);

    // Configure pins
    //selectPinPushPullOutput(?????);

}

int main(void)
{
    USER_DATA data;
    initHW();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    putsUart0("\n>");

    bool valid;
    while (true)
    {
        if (kbhitUart0())
        {
            //process command
            getsUart0(&data);
            if (enterPressed)
            {
                enterPressed = false;
                parseFields(&data);

                valid = false;

                if (isCommand(&data, "temperature", 0))
                {
                    valid = true;
                }

                //for easy testing purposes
                else if (isCommand(&data, "test", 0))
                {
                    valid = true;
                }

                //classic help command, will give the user instructions
                //on how to use the utility
                else if (isCommand(&data, "help", 0))
                {

                    valid = true;
                }

                //will let the user know if the command they entered didnt exist
                if (!valid)
                {
                    putsUart0("Invalid command\n\r");
                }
                putsUart0(">");
            }
        }
    }
}
