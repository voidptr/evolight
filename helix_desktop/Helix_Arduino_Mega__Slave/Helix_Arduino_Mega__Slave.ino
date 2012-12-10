#include <FastSPI_LED.h>
#include <CapacitiveSensor.h>
#include "controller.h"

#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

Controller * Control;

void setup() 
{

    Control = new Controller();

    debug_setup();
    Control->init();

}

void loop() 
{ 
    Control->program_loop();
}

void serialEvent1()
{
    Control->read_serial();
}
