#include <FastSPI_LED.h>
#include "controller.h"

#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

//struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
//struct CRGB *leds;

Controller * Control;

void setup() {

//    FastSPI_LED.setLeds(120);
//    FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2811);
//    FastSPI_LED.setPin(6);
//    FastSPI_LED.init();
//    FastSPI_LED.start();
//    leds = (struct CRGB*)FastSPI_LED.getRGBData();

    Control = new Controller();

    debug_setup();
    Control->init();
}

void loop() { 
    debug();
    Control->program_loop();

    debug_thing();

}

void debug_thing() 
{
//    for(int j = 0; j < 3; j++) {   
//    for(int i = 0 ; i < 120; i++ ) {
//      memset(leds, 0, 120 * 3);
//      switch(j) { 
//        case 0: leds[i].r = 255; break;
//        case 1: leds[i].g = 255; break;
//        case 2: leds[i].b = 255; break;
//      }
//      FastSPI_LED.show();
//      delay(10);
//    }
//  }
}



