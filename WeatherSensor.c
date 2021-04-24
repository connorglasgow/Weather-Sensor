//Connor Glasgow
//1001322122

//----------------------------------------
//  Includes, etc.
//----------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "uart0.h"
#include "wait.h"
#include "gpio.h"
#include "i2c0.h"
#include "commandline.h"

//----------------------------------------
// TEMP GLOBAL VARS
//----------------------------------------
//for commandline interface
extern bool enterPressed;
//hardcoded i2c address for sensor
uint8_t sensorAdd = 118;

//----------------------------------------
//  Functions
//----------------------------------------

//not actually using this right now
void initHW()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();
}

//function yoinked from Dr.Losh's i2c utility program
//used to read and write hex or decimal address
uint8_t asciiToUint8(const char str[])
{
    uint8_t data;
    if (str[0] == '0' && tolower(str[1]) == 'x')
        sscanf(str, "%hhx", &data);
    else
        sscanf(str, "%hhu", &data);
    return data;
}

void readTemp()
{

}

int main(void)
{
    USER_DATA userData;
    initSystemClockTo40Mhz();
    initUart0();
    initI2c0();
    setUart0BaudRate(115200, 40e6);

    //some variables
    char outstr[100];
    char *token;
    uint8_t add;
    uint8_t reg;
    uint8_t data;
    uint8_t arg;
    bool valid;

    putsUart0("Weather Sensor\n>");

    while (true)
    {
        if (kbhitUart0())
        {
            //process command
            getsUart0(&userData);
            if (enterPressed)
            {
                enterPressed = false;
                parseFields(&userData);

                valid = false;

                if (isCommand(&userData, "temperature", 0))
                {
                    token = "0x76";
                    add = asciiToUint8(token);

                    readTemp();
                    valid = true;
                }

                if (isCommand(&userData, "humidity", 0))
                {
                    valid = true;
                }

                if (isCommand(&userData, "pressure", 0))
                {
                    valid = true;
                }

                //for easy testing purposes
                else if (isCommand(&userData, "test", 0))
                {
                    putsUart0("Showing Address of the BME280: ");
                    data = readI2c0Register(sensorAdd, 208);
                    sprintf(outstr, "data is 0x%x or %u\n", data, data);
                    putsUart0(outstr);

                    valid = true;
                }

                //classic help command, will give the user instructions
                //on how to use the utility
                else if (isCommand(&userData, "help", 0))
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
