//
//  TouchController.h
//  Helix
//
//  Created by Rosangela Canino Koning on 2/7/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "IDEIncludes.h"

#ifndef __Helix__TouchController__
#define __Helix__TouchController__

//#include "Wire.h"
#include "BinaryOutput.h"
#include "CypressCapsense.h"
#include "Debug.h"


#define Port0 0b0000010000000000 //12
#define Port1 0b0000000000010000 //04
#define Port2 0b0000000000001000 //03
#define Port3 0b0000000000000100 //02
#define Port4 0b0000000000000010 //01
#define Port5 0b0000000000000001 //00
#define Port6 0b0000001000000000 //11
#define Port7 0b0000000100000000 //10
#define Port8 0b0001000000000000 //13
#define Port9 0b0000100000000000 //14

class TouchController
{
private:
    uint16_t Ports[10];
    //byte addr;

    byte controllerCount;
    
public:
    byte FoundControllerAddresses[128];    
public:
    TouchController();
    void init();
    void debug_loop();
    void get_capsense_firmware(byte addr);
    void setup_scan();
    bool test_for_device(byte addr);
    bool configure_device(byte addr);
    int fetch_sensor_status(byte addr);
    bool status_by_port(int aRawVal, int aPort);
    bool output_device_status(byte addr);
 
    
};
#endif /* defined(__Helix__TouchController__) */
