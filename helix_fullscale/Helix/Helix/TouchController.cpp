//
//  TouchController.cpp
//  Helix
//
//  Created by Rosangela Canino Koning on 2/7/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "TouchController.h"

TouchController::TouchController()
{
    Ports = {Port0, Port1, Port2, Port3, Port4, Port5, Port6, Port7, Port8, Port9};
    controllerCount = 0;
}

void TouchController::get_capsense_firmware(byte addr)
{
    char buf[1];
    buf[0] = 0xFF;
    CypressCapsenseC8YC20.fetchFirmwareRevision(addr, buf);
    BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1);
}

void TouchController::init()
{
    BINARYOUTPUT.begin();

    setup_scan();
    
}

void TouchController::debug_loop()
{
    char stringBuf[20];

    for (int i = 0; i < controllerCount; i++)
    {
        byte addr = FoundControllerAddresses[i];
        int stat = fetch_sensor_status(addr);

        BINARYOUTPUT.int_to_binary_string(stat, stringBuf);
        Serial.print("Touch Status: ");
        Serial.print((int)addr);
        Serial.print(" ");
        Serial.println(stringBuf);
    }
}

void TouchController::setup_scan()
{
    byte addr = 0x00;
    for (int i = 0; i < 100; i++)
    {
        if (test_for_device(addr))
        {
            Debug::debug("FOUND (int): ");
            Debug::debugln(i);
            
            configure_device(addr);
            
            FoundControllerAddresses[controllerCount] = addr;
            controllerCount++;
        }
        addr++;
    }
}

bool TouchController::test_for_device(byte addr)
{
    char buf[1];
    buf[0] = 0xFF;
    
    Debug::debug("Testing Address: ");
    Debug::debugln((int)addr);
    
    // fetch the firmware revision. This will indicate if the device is listening at the default address
    CypressCapsenseC8YC20.fetchFirmwareRevision(addr, buf);
    byte response = buf[0];
    
    if (response != 0xFF)
    {
        Debug::debugln("FOUND A DEVICE. Version: ");
        BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1);

        
        return true;
    }
    return false;
}

bool TouchController::configure_device(byte addr)
{
    byte configtarget = addr;
    
    Debug::debug("CONFIGURING DEVICE @ ");
    Debug::debugln((int)configtarget);
    // set up the device to read capsense on all ports.
    CypressCapsenseC8YC20.setupDevice(configtarget, 0x0000,
                                      (Port0 | Port1 | Port2 | Port3 | Port4 | Port5 | Port6 | Port7 | Port8 | Port9),
                                      0, NULL);
    
    return true;
}

int TouchController::fetch_sensor_status(byte addr)
{
    int status = CypressCapsenseC8YC20.fetchTouchStatus(addr);
    CypressCapsenseC8YC20.reset(addr);
    return status;
}

bool TouchController::status_by_port(int aRawVal, int aPort)
{
    return (Ports[aPort] & aRawVal);
}

bool TouchController::output_device_status(byte addr)
{
    char stringBuf[20];
    int touchstatus = CypressCapsenseC8YC20.fetchTouchStatus(addr);
    BINARYOUTPUT.int_to_binary_string(touchstatus, stringBuf);
    Serial.print("Touch Status: ");
    Serial.println(stringBuf);
    
    Serial.println("Raw Counts: ");
    Serial.println("");
    for (int i = 0; i < 10; i++)
    {
        int ct = CypressCapsenseC8YC20.fetchRawCounts(addr, Ports[i]);
        int status = (touchstatus & Ports[i]) > 0 ? 1 : 0;
        Serial.println("Port: "
                       + String(i)
                       + "("
                       //+ String( Ports[i])
                       + ") -------------- "
                       + String(ct)
                       + "-----"
                       + String(status));
    }
    
}
