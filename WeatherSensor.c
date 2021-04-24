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
//device driver from manufacturer
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
void print_sensor_data(struct bme280_data *comp_data)
{
    int32_t temp;
    int32_t press;
    int32_t hum;
    char outstr[100];

    temp = 0.01f * comp_data->temperature;
    press = 0.0001f * comp_data->pressure;
    hum = 1.0f / 1024.0f * comp_data->humidity;

    sprintf(outstr, "%0.2lf deg C,  %0.2lf hPa, %0.2lf%% \n", temp, press, hum);
    putsUart0(outstr);
}

int8_t read_sensor_data(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t settings_sel;
    struct bme280_data comp_data;

    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_1X;
    dev->settings.osr_t = BME280_OVERSAMPLING_1X;
    dev->settings.filter = BME280_FILTER_COEFF_OFF;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL
            | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if (rslt != BME280_OK)
    {
        putsUart0("Error: set_sensor_settings\n");
    }
    putsUart0("Temperature, Pressure, Humdity\n");

    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
    if (rslt != BME280_OK)
    {
        putsUart0("Error: set_sensor_mode\n");
    }

    dev->delay_us(40000, dev->intf_ptr);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
    if (rslt != BME280_OK)
    {
        putsUart0("Error: get_sensor_data\n");
    }

    print_sensor_data(&comp_data);
    dev->delay_us(1000000, dev->intf_ptr);

    return rslt;
}

void delay_us(uint32_t period, void *intf_ptr)
{
    waitMicrosecond(period);
}

int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                void *intf_ptr)
{
    *reg_data = readI2c0Register(sensorAdd, reg_addr);
    return 0;
}

int8_t i2c_write(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                 void *intf_ptr)
{
    writeI2c0Register(sensorAdd, reg_addr, reg_data);
    return 0;
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
    uint8_t msb;
    uint8_t lsb;
    uint8_t xsb;

    msb = readI2c0Register(sensorAdd, 250);
    lsb = readI2c0Register(sensorAdd, 251);
    xsb = readI2c0Register(sensorAdd, 252);

    waitMicrosecond(100);
}

void readHumidity()
{
    uint8_t msb;
    uint8_t lsb;
    uint8_t xsb;

    msb = readI2c0Register(sensorAdd, 250);
    lsb = readI2c0Register(sensorAdd, 251);
    xsb = readI2c0Register(sensorAdd, 252);

    waitMicrosecond(100);

}

void readPressure()
{
    uint8_t msb;
    uint8_t lsb;
    uint8_t xsb;

    msb = readI2c0Register(sensorAdd, 250);
    lsb = readI2c0Register(sensorAdd, 251);
    xsb = readI2c0Register(sensorAdd, 252);

    waitMicrosecond(100);
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
    uint8_t data8;
    uint16_t data16;
    uint32_t data32;
    uint8_t arg;
    uint32_t us;
    bool valid;

    //initialization stuff for the device
    struct bme280_data comp_data;
    struct bme280_dev dev;
    int8_t rslt = BME280_OK;
    uint8_t device_add = BME280_I2C_ADDR_PRIM;

    dev.intf_ptr = &device_add;
    dev.intf = BME280_I2C_INTF;

    //temp comment for testing
    dev.read = i2c_read;

    //temp comment out for testing
    dev.write = i2c_write;

    //temp comment out for testing
    dev.delay_us = delay_us;

    rslt = bme280_init(&dev);

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
                    readHumidity();
                    valid = true;
                }

                if (isCommand(&userData, "pressure", 0))
                {
                    readPressure();
                    valid = true;
                }

                if (isCommand(&userData, "read", 0))
                {
                    rslt = read_sensor_data(&dev);
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
