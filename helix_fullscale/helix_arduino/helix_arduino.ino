#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include <Wire.h>
#include <IOShieldOled.h>


//#include <FastSPI_LED.h>
#include "controller.h"

#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

Controller Control;

void setup() 
{

    debug_setup();
    debug("sketch setup");
    Control.init();
    debug("end sketch setup");

}

void loop() 
{ 
    debug();
    debug("HI");
  
    //digitalWrite(PIN_LED1, HIGH); // set the LED on
    //delay(1000);                  // wait for a second
    //digitalWrite(PIN_LED1, LOW);  // set the LED off
    //delay(1000); 
    Control.program_loop();
}

