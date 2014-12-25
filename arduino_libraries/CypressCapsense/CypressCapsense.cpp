/*
   CypressCapsense.cpp - Cypress Capsense Express C8YC20 Module
   Copyright (c) 2012 Rosangela Canino-Koning.  All right reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
   Cypress Capsense Express C8YC20 IC I2C Bus Communication
   Developed for the ChipKit microcontroller boards by Digilent Inc.

   Library based on example IOShieldEEPROMClass, Copyright (c) 2011 Digilent Inc.
   Commands based on Cypress Capsense example code, Copyright (c) 2011 Nanwei Gong & Nan Zhao
 */


extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
    #define WIREWRITE Wire.write
    #define WIREREAD Wire.read
#else
    #include "WProgram.h"
    #define WIREWRITE Wire.send
    #define WIREREAD Wire.receive
#endif

#include "Wire.h"

//#include <plib.h>
#include "CypressCapsense.h"

CypressCapsenseC8YC20_Class::CypressCapsenseC8YC20_Class()
{
    unlock[0] = 0x3C;
    unlock[1] = 0xA5;
    unlock[2] = 0x69;
    //unlock = {0x3C, 0xA5, 0x69}; // unlock command
    //lock =   {0x96, 0x5A, 0xC3}; // lock command
    lock[0] = 0x96;
    lock[1] = 0x5A;
    lock[2] = 0xC3;


//#if defined(_BOARD_MEGA_)
//    PORTSetPinsDigitalIn(IOPORT_B, BIT_4 | BIT_5);
//#endif
    Wire.begin();
}

/* ------------------------------------------------------------ */
/***	uint8_t CypressCapsenseC8YC20_Class::read(uint8_t device_address, uint8_t register_address)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **		register_address		- 8 Bit register address designating were to read
 **
 **	Return Value:
 **		byte
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Splits the 16 bit register address into two bytes and sends them
 **		to the Capsense device. Next it reads from the Capsense device
 **		to get the data at that location and returns a byte
 **
 */
uint8_t CypressCapsenseC8YC20_Class::read(uint8_t device_address, uint8_t register_address)
{
    uint8_t temp = 0;
    readString(device_address, register_address, &temp, 1);
    return temp;
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::readString(
 **          uint8_t device_address, 
 **          uint16_t register_address, 
 **          uint8_t *sz, 
 **          int size)
 **
 **	Parameters:
 **     device_address          - 7 Bit device address designating the device we want to talk to
 **		register_address		- 8 Bit register_address designating where to read
 **		buf			            - pointer to string of bytes
 **		size		            - number of bytes to be read
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Splits the 16 bit register_address into two bytes and sends them
 **		to the Capsense device. Next it reads consecutive bytes starting
 **		from the given register_address and stores them in the location provided
 **
 */
void CypressCapsenseC8YC20_Class::readString(
        uint8_t device_address, uint8_t register_address, char *buf, int size)
{
    readString(device_address, register_address, (uint8_t*)buf, size);
}

void CypressCapsenseC8YC20_Class::readString(
        uint8_t device_address, uint8_t register_address, uint8_t *buf, int size)
{
    //	uint8_t temp[2] = {0,0};

    //	temp[0] = (register_address >> 8);
    //	temp[1] = (register_address & 0xFF);

    //Serial.println("TALKING");
    //digitalWrite(7, HIGH); // set the LED on

    Wire.beginTransmission((int)device_address);
    WIREWRITE(&register_address, 1);
    uint8_t status = Wire.endTransmission();
    //Serial.println("Status (endtrans): " + String((int)status));

    status = Wire.requestFrom((int)device_address, size);
    //Serial.println("Bytes read (reqfrom): " + String((int)status));


    //Serial.println("DONE TALKING");
    //digitalWrite(7, LOW);  // set the LED off

    int index = 0;
    while(Wire.available())
    {
        //Serial.println("Read byte");
        buf[index++] = WIREREAD();
    }


}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::readI2CBuffer(
 **          uint8_t device_address, 
 **          uint8_t *buf, 
 **          int size)
 **
 **	Parameters:
 **     device_address          - 7 Bit device address designating the device we want to talk to
 **		buf			            - pointer to string of Bytes
 **		size		            - amount of data to be read
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **      Read from the I2C buffer whatever amount of data is available. It is assumed that this
 **      buffer was primed in a previous separate command.
 **
 */
void CypressCapsenseC8YC20_Class::readI2CBuffer(uint8_t device_address, char *buf, int size)
{	
    readI2CBuffer(device_address, (uint8_t*)buf, size);
}

void CypressCapsenseC8YC20_Class::readI2CBuffer(uint8_t device_address, uint8_t *buf, int size)
{	
    //Serial.println("TALKING");
    //digitalWrite(7, HIGH); // set the LED on

    uint8_t status = Wire.requestFrom((int)device_address, size);
    //Serial.println("Bytes read (reqfrom): " + String((int)status));

    //Serial.println("DONE TALKING");
    //digitalWrite(7, LOW); // set the LED on


    int index = 0;
    while(Wire.available())
    {
        buf[index++] = WIREREAD();
    }
}


/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::write(
 **          uint16_t device_address, uint16_t register_address, uint8_t data)
 **
 **	Parameters:
 **     device_address          - 7 Bit device address designating the device we want to talk to
 **		register_address		- 8 Bit register_address designating were to write
 **		data		            - Data to be written
 **
 **	Return Value:
 **
 **	Errors:
 **
 **	Description:
 **		Splits the 16 bit register_address into two bytes and sends them
 **		to the Capsense device. Then writes a byte to specified register_address
 **
 */
void CypressCapsenseC8YC20_Class::write(uint8_t device_address, uint8_t register_address, uint8_t data)
{
    writeString(device_address, register_address, &data, 1);
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::writeString(
 **          uint8_t device_address, uint16_t register_address, uint8_t *data, int size)
 **
 **	Parameters:
 **     device_address          - 7 Bit device address designating the device we want to talk to
 **		register_address		- 8 Bit register_address designating were to write
 **		data		- Data to be written
 **		size		- Bytes to be written
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **		Splits the 16 bit register_address into two bytes and sends them
 **		to the Capsense device. Then writes a string of bytes to 
 **		specified register_address. If amount of bytes is above 64 the data
 **		will automaticly be truncated.
 **
 */
void CypressCapsenseC8YC20_Class::writeString(
        uint8_t device_address, uint8_t register_address, char *data, int size)
{
    writeString(device_address, register_address, (uint8_t*)data, size);
}

void CypressCapsenseC8YC20_Class::writeString(
        uint8_t device_address, uint8_t register_address, char *data)
{
    writeString(device_address, register_address, data, strlen(data));
}

void CypressCapsenseC8YC20_Class::writeString(
        uint8_t device_address, uint8_t register_address, uint8_t *data, int size)
{
    int i;
    uint8_t temp[67] = {0,0,0};
    temp[0] = register_address;
    //	temp[1] = (register_address & 0xFF);

    if(size > 64) // why?
    {
        size = 64;
    }

    for(i=0; i<size; i++)
    {
        temp[i+1] = *data;
        data++;
    }

    //Serial.println("TALKING");
    //digitalWrite(7, HIGH); // set the LED on

    Wire.beginTransmission((int)device_address);
    WIREWRITE(temp, (size + 1));
    uint8_t status = Wire.endTransmission();
    //Serial.println("Status (endtrans): " + String((int)status));

    //Serial.println("DONE TALKING");
    //digitalWrite(7, LOW); // set the LED on


    delay(10);
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::changeDeviceAddress(
 **          uint8_t device_address, uint8_t new_address)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **		new_address		        - 7 Bit new device address to set
 *
 **	Return Value:
 **		none
 **
 **	Errors:
 **		none
 **
 **	Description:
 **      Sends command to device to unlock the address register, change the address
 **      value, and re-lock the register.
 **
 */
void CypressCapsenseC8YC20_Class::changeDeviceAddress(uint8_t device_address, uint8_t new_address)
{
    writeString(device_address, CSE_I2C_DEV_LOCK, unlock, 3); // unlock

    // ORed with MSB 1 to set open drain
    write(device_address, CSE_I2C_ADDR_DM, (0b10000000|new_address));

    writeString(device_address, CSE_I2C_DEV_LOCK, lock, 3); // lock
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::setupDevice(
 **          uint8_t device_address, 
 **          uint8_t gpio0, uint8_t gpio1, uint8_t capsense0, uint8_t capsense1)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **		gpio		            - 16 bits to turn on as GPIO xxxBBBBBxxxBBBBB
 **                                  (LSB 8 bits (LSB 5 bits) for GPIO0, MSB 8 bits (LSB 5 bits) GPIO1)
 **  	capsense		        - 16 bits to turn on as Capsense xxxBBBBBxxxBBBBB
 **                                  (LSB 8 bits (LSB 5 bits) for GPIO0, MSB 8 bits (LSB 5 bits) GPIO1)
 **
 **	Return Value:
 **		none
 **
 **	Errors:
 **		If the pin settings conflict, return false
 **
 **	Description:
 **      Sets the pins to be used as capsense or GPIO on ports 0 and 1. A pin may not be
 **      both capsense and gpio at the same time.
 **
 */
bool CypressCapsenseC8YC20_Class::setupDevice(
        uint8_t device_address, 
        uint16_t gpio, uint16_t capsense)
{
    uint8_t gpio0 = (gpio & 0xFF); //LSB
    uint8_t gpio1 = (gpio >> 8); //MSB

    uint8_t capsense0 = (capsense & 0xFF); //LSB
    uint8_t capsense1 = (capsense >> 8); //MSB


    // check for error condition!
    if ((gpio0 & capsense0 > 0x00) || (gpio1 & capsense1 > 0x00))
        return false;    

    write(device_address, CSE_COMMAND_REG, SETUP_OPERATION_MODE);

    // wipe the previous setting.
    write(device_address, CSE_GPIO_ENABLE0, 0x00);
    write(device_address, CSE_GPIO_ENABLE1, 0x00);
    write(device_address, CSE_CS_ENABLE0, 0x00);
    write(device_address, CSE_CS_ENABLE1, 0x00);

    write(device_address, CSE_GPIO_ENABLE0, gpio0);
    write(device_address, CSE_GPIO_ENABLE1, gpio1);
    write(device_address, CSE_CS_ENABLE0, capsense0);
    write(device_address, CSE_CS_ENABLE1, capsense1);
    
//    write(device_address, CSE_CS_OTH_SET, (CSE_OTH_SET_ENABLE_EXT_CAP | CSE_OTH_SET_NO_SENSOR_RESET | CSE_OTH_SET_CLOCK_IMO2));
    write(device_address, CSE_CS_OTH_SET, (CSE_OTH_SET_DISABLE_EXT_CAP | CSE_OTH_SET_SENSOR_RESET | CSE_OTH_SET_CLOCK_IMO));
    
    write(device_address, CSE_CS_IDAC_00, 15);
    write(device_address, CSE_CS_IDAC_01, 15);
    write(device_address, CSE_CS_IDAC_02, 15);
    write(device_address, CSE_CS_IDAC_03, 15);
    write(device_address, CSE_CS_IDAC_04, 15);
    write(device_address, CSE_CS_IDAC_10, 15);
    write(device_address, CSE_CS_IDAC_11, 15);
    write(device_address, CSE_CS_IDAC_12, 15);
    write(device_address, CSE_CS_IDAC_13, 15);
    write(device_address, CSE_CS_IDAC_14, 15);
    
    
    write(device_address, CSE_COMMAND_REG, STORE_CURRENT_CONFIGURATION_TO_NVM);
    write(device_address, CSE_COMMAND_REG, RECONFIGURE_DEVICE);
    write(device_address, CSE_COMMAND_REG, NORMAL_OPERATION_MODE);

    return true;
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::fetchTouchStatus(
 **          uint8_t device_address)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **
 **	Return Value:
 **		uint16_t                - The status of the sensors. Each bit being on/off
 **
 **	Errors:
 **		none
 **
 **	Description:
 **      Fetches the touch true/false status from the sensors
 **
 */
uint16_t CypressCapsenseC8YC20_Class::fetchTouchStatus(uint8_t device_address)
{
    uint16_t tmp = 0;
    //Serial.println("CSE_CS_READ_STATUS0");
    tmp = (read(device_address, CSE_CS_READ_STATUS0)) << 8;
    //Serial.println("CSE_CS_READ_STATUS1");
    tmp |= read(device_address, CSE_CS_READ_STATUS1);

    return tmp;
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::fetchTouchStatus(
 **          uint8_t device_address, uint8_t port, uint8_t sensor)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **      port                    - Which port to query (0 or 1)
 **      sensor                  - Which sensor to query (0-4)
 **
 **	Return Value:
 **		uint16_t                - The raw count of the sensor. 
 **
 **	Errors:
 **		If port > 1, or sensor > 4, return 0
 **
 **	Description:
 **      Fetches the touch true/false status from the sensors
 **
 */
uint16_t CypressCapsenseC8YC20_Class::fetchRawCounts(uint8_t device_address, uint16_t port)
{
    uint8_t capsensePort = 0;
    uint8_t capsenseSensor = port;
    if (port > 255)
    {
        capsenseSensor = (port >> 8);
        capsensePort = 1;
    }   

    uint8_t port_sensor_select = (capsensePort << 7) | (capsenseSensor);
    write(device_address, CSE_CS_READ_BUTTON, port_sensor_select);


    uint16_t tmp = 0;
    tmp = ((uint16_t)(read(device_address, CSE_CS_READ_RAWM))) << 8;
    tmp |= read(device_address, CSE_CS_READ_RAWL);

    return tmp;
}

void CypressCapsenseC8YC20_Class::reset(uint8_t device_address)
{
    write(device_address, CSE_CS_FILTERING, CS_FILTERING_TOUCH_BASELINE_RESET);
}

/* ------------------------------------------------------------ */
/***	void CypressCapsenseC8YC20_Class::getDeviceInformation(
 **          uint8_t device_address, 
 **          uint8_t *buffer, uint8_t size)
 **
 **	Parameters:
 **      device_address          - 7 Bit device address designating the device we want to talk to
 **		buffer		            - The buffer to write the data to. It should be at least 124 bytes
 **		size		            - The size of the buffer
 **
 **	Return Value:
 **		bool                    - success or failure to read the thing
 **
 **	Errors:
 **		If size < 124 bytes, return false
 **
 **	Description:
 **      Sets the pins to be used as capsense or GPIO on ports 0 and 1. A pin may not be
 **      both capsense and gpio at the same time.
 **
 */
bool CypressCapsenseC8YC20_Class::fetchDeviceInformation(uint8_t device_address, char *buffer, uint8_t size)
{
    return fetchDeviceInformation(device_address, (uint8_t*) buffer, size);
}

bool CypressCapsenseC8YC20_Class::fetchDeviceInformation(uint8_t device_address, uint8_t *buffer, uint8_t size)
{
    if (size < 124)
        return false;

    write(device_address, CSE_COMMAND_REG, GET_FIRMWARE_REVISION);
    readI2CBuffer(device_address, buffer, 1);

    write(device_address, CSE_COMMAND_REG, READ_DEVICE_CONFIGURATION);
    readI2CBuffer(device_address, buffer+1, 123);

    return true;
}
bool CypressCapsenseC8YC20_Class::fetchFirmwareRevision(uint8_t device_address, char *buffer)
{
    return fetchFirmwareRevision(device_address, (uint8_t*) buffer);
}

bool CypressCapsenseC8YC20_Class::fetchFirmwareRevision(uint8_t device_address, uint8_t *buffer)
{
    write(device_address, CSE_COMMAND_REG, GET_FIRMWARE_REVISION);
    readI2CBuffer(device_address, buffer, 1);

    return true;
}

CypressCapsenseC8YC20_Class CypressCapsenseC8YC20 = CypressCapsenseC8YC20_Class();
