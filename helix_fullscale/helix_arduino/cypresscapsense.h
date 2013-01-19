/*
  CypressCapsense.h - Cypress Capsense Express C8YC20 IC
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

#ifndef CYPRESS_CAPSENSE_H
#define CYPRESS_CAPSENSE_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include <inttypes.h>

#ifndef DEBUG
#define DEBUG 0 
#endif


/* Register documentation for Cypress Capsense Express CY8C201xx found in 
Cypress CY8C201xx Register Reference Guide 
Spec. # 001-45146 Rev. *D, April 27, 2011 */


// register addresses
#define CSE_CS_ENABLE0 0x06
#define CSE_CS_ENABLE1 0x07

#define CSE_GPIO_ENABLE0 0x08
#define CSE_GPIO_ENABLE1 0x09

#define CSE_I2C_DEV_LOCK 0x79 
#define CSE_I2C_ADDR_DM 0x7C
#define CSE_COMMAND_REG 0xA0

#define CSE_CS_READ_BUTTON 0x81 // which button shows up in raw counts.

#define CSE_CS_READ_RAWM 0x86 // raw counts
#define CSE_CS_READ_RAWL 0x87

#define CSE_CS_READ_STATUS0 0x88 // on off
#define CSE_CS_READ_STATUS1 0x89

// commands
#define SETUP_OPERATION_MODE 0x08
#define STORE_CURRENT_CONFIGURATION_TO_NVM 0x01
#define RECONFIGURE_DEVICE 0x06
#define NORMAL_OPERATION_MODE 0x07
#define READ_DEVICE_CONFIGURATION 0x05
#define GET_FIRMWARE_REVISION 0x00



class CypressCapsenseC8YC20_Class
{
    
  private:
    uint8_t unlock[3];
    uint8_t lock[3];


  public:
	CypressCapsenseC8YC20_Class();
    /*
    uint8_t read(uint8_t device_address, uint8_t register_address);
	void readString(uint8_t device_address, uint8_t register_address, char *buf, int size);
	void readString(uint8_t device_address, uint8_t register_address, uint8_t *buf, int size);
    void readI2CBuffer(uint8_t device_address, char *buf, int size);
    void readI2CBuffer(uint8_t device_address, uint8_t *buf, int size);
    void write(uint8_t device_address, uint8_t register_address, uint8_t data);
	void writeString(uint8_t device_address, uint8_t register_address, char *data, int size);
	void writeString(uint8_t device_address, uint8_t register_address, char *data);
	void writeString(uint8_t device_address, uint8_t register_address, uint8_t *data, int size);

    void changeDeviceAddress(uint8_t device_address, uint8_t new_address);
    bool setupDevice(uint8_t device_address, uint16_t gpio, uint16_t capsense);
    uint16_t fetchTouchStatus(uint8_t device_address);
    uint16_t fetchRawCounts(uint8_t device_address, uint16_t port);
    bool fetchDeviceInformation(uint8_t device_address, char *buffer, uint8_t size);
    bool fetchDeviceInformation(uint8_t device_address, uint8_t *buffer, uint8_t size);
    bool fetchFirmwareRevision(uint8_t device_address, char *buffer);
    bool fetchFirmwareRevision(uint8_t device_address, uint8_t *buffer);
    */

};

extern CypressCapsenseC8YC20_Class CypressCapsenseC8YC20;



#endif


