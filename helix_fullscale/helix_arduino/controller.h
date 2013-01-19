
// processing
//#include "lights_via_fastspi.h"
#include "oled_io.h"
#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class Controller 
{
private:

public:
  //GridLightControl Lights;
  OLED_IO Screen;

public:
  // constructor, pass in the basic crap so we can pass it on to our minions.
  Controller() 
  {
//    debug("Controller CTOR");


//    debug("END Controller CTOR");
  }

  void init() 
  {
    debug(" Controller init()");

    //Lights.init();
    Screen.init();



    debug(" END Controller init()");
  }

  // do the output
  void program_loop() 
  {

    //    populate_lights_array();
    Screen.output();
  }


private:

};




