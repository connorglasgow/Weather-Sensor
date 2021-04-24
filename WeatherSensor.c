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
#include "bme280.h"

//----------------------------------------
// TEMP GLOBAL VARS
//----------------------------------------
//for commandline interface
extern bool enterPressed;
//hardcoded i2c address for sensor
uint8_t sensorAdd = 118;

//----------------------------------------
//  Data sheet recommended settings
//----------------------------------------
/*  for 'weather monitoring' which is what we're doing here
 *  Sensor mode: forced mode, 1 sample/second
 *  Oversampling settings: pressure*0, temperature*1,humidity*1
 *  IIR filter settings: filter off
 *  should be about a 1Hz data output rate
 */

//----------------------------------------
//  Functions
//----------------------------------------

//not actually using this right now
void initHW()
{

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

void readHumidity()
{

}

void readPressure()
{

}

int main(void)
{
    USER_DATA userData;
    initSystemClockTo40Mhz();
    initUart0();
    initI2c0();
    initHw();
    setUart0BaudRate(115200, 40e6);

    //some variables
    char outstr[100];
    char *token;
    uint8_t add;
    uint8_t reg;
    uint8_t data8;
    uint16_t data16;
    uint32_t data32;
    uint8_t arg;
    bool valid;

    //initialization stuff for the device
    DEVICE device;


    putsUart0("\nWeather Sensor\n>");

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
                    putsUart0(
                            "Showing ID of the BME280 from the \"id\" register\n");
                    data8 = readI2c0Register(sensorAdd, 208);
                    data16 = readI2c0Register(sensorAdd, 208);
                    data32 = readI2c0Register(sensorAdd, 208);
                    sprintf(outstr, "data is 0x%x or %u\n"
                            "add is 0x%x or %u\n"
                            "register is 0x%x or %u\n",
                            data8, data8, sensorAdd, sensorAdd, 208, 208);
                    putsUart0(outstr);

                    valid = true;
                }

                //classic help command, will give the user instructions
                //on how to use the utility
                else if (isCommand(&userData, "help", 0))
                {

                    valid = true;
                }

                //will let the user know if the command they entered doesn't exist
                if (!valid)
                {
                    putsUart0("Invalid command\n\r");
                }
                putsUart0(">");
            }
        }
    }
}
