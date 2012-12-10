#include "Arduino.h"

// processing
#include "lights_via_fastspi.h"
#include "cap_buttons.h"
//#include "serial_communications.h"
#include "serial_async.h"

#ifndef DEBUG_H
  #include "debug.h"
  #define DEBUG_H
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class Controller 
{
private:
    int strands;
    int strand_showing;

    int bad_count;

public:
    GridLightControl Lights;
    Cap_Buttons Buttons;
    Serial_Communications * Comms;
    

public:
    // constructor, pass in the basic crap so we can pass it on to our minions.
    Controller() 
    {
        debug("Controller CTOR");

        strands = Lights.get_strands();

        Comms = new Serial_Communications();

        strand_showing = 0;

        bad_count = 0;

        debug("END Controller CTOR");
    }

    void init() 
    {
        debug("Controller init()");
       
        Lights.init();
        Comms->init();

        debug("END Controller init()");
    }

    void softReset()
    {
        debug("soft reset!");
        Comms->dump_buffer("Too many bad counts.");
        asm volatile ("  jmp 0");
    }

    // do the output
    void program_loop() 
    {

        if (bad_count > 20)
            softReset();

        int ct = 0;
        while (Serial1.available() == 0 && ct++ < strands) // only do this if it makes sense
        {
            //Serial.print(".");
            Lights.show(strand_showing);
            strand_showing = ++strand_showing % strands;

            delay(1);
        }
//        Serial.println("");

    }

    // process serial data
    void read_serial()
    {
        Comms->pull_data();
        populate_lights_array();
    }


private:
    void populate_lights_array()
    {
        if (Comms->m_BytesRead % 5 > 0)
        {
            debug("Bad Count");
            bad_count++;
            return;
        }
        for (int i = 0; i < Comms->m_BytesRead; i += 5) // 5 is the packet size
        {
            if (!(Lights.set_light( (unsigned int)((Comms->InBuffer[i] << 8) + Comms->InBuffer[i+1]), // light index
                Comms->InBuffer[i+2],  // r
                Comms->InBuffer[i+3],  // g
                Comms->InBuffer[i+4])))// b
                {
                    bad_count++;
                    //debug("~~~~~bad light --- ");
                    //debug(i/5);
                    //debug((unsigned int)((Comms->InBuffer[i] << 8) + Comms->InBuffer[i+1]));
                    //debug(Comms->InBuffer[i]);
                    //debug(Comms->InBuffer[i+1]);
                    //debug(Comms->InBuffer[i+2]);
                    //debug(Comms->InBuffer[i+3]);
                    //debug(Comms->InBuffer[i+4]);
                    //debug("####### PREV ######");
                    //debug((unsigned int)((Comms->InBuffer[i-5] << 8) + Comms->InBuffer[i-4]));
                    //debug(Comms->InBuffer[i-5]);
                    //debug(Comms->InBuffer[i-4]);
                    //debug(Comms->InBuffer[i-3]);
                    //debug(Comms->InBuffer[i-2]);
                    //debug(Comms->InBuffer[i-1]);

                    //debug("~~~~~~~~~~~~~~~~~~~");
                }
        }
    }
};

