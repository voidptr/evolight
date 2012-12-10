#ifndef DEBUG_H
  #include "debug.h"
  #define DEBUG_H
#endif

#define __ORIENTATION__ 0x01 // That is, linear
#define __NUM_STRANDS__ 11
#define __NUM_STATUS_LIGHTS__ 10

#define __TOTAL_LIGHT_COUNT__ 300
#define __TOTAL_ORGANISM_LIGHTS__ 290 // and they are the first 290

class GridLightControl 
{
private:
    struct CRGB { unsigned char g; unsigned char r; unsigned char b; }; // for whatever reason, the order is wrong here.
    struct CRGB * leds;

    enum 
    {
        LINEAR_ORIENTATION = 0x01,
        ZIGZAG_ORIENTATION = 0x02
    }; 

public:
    // constructor
    GridLightControl() 
    {
    }

    void init() 
    {
        FastSPI_LED.setLeds(__TOTAL_LIGHT_COUNT__);
        FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2811);

        // THIS BIT HERE IS SUPER GHETTO -
        // --- UPDATE THIS IF YOU ADD MORE STRANDS ZOMG ---
        FastSPI_LED.setPinCount(11); 
        FastSPI_LED.setPin(0, 4, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(1, 5, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(2, 6, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(3, 7, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(4, 8, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(5, 9, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(6, 10, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(7, 11, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(8, 12, 29); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(9, 13, 29); // this version DOES NOT do a malloc, so no memory leak

        // status lights -- not part of the 10x29 grid
        FastSPI_LED.setPin(10, 3, 10); // this version DOES NOT do a malloc, so no memory leak
        
        FastSPI_LED.init();
        FastSPI_LED.start();
        leds = (struct CRGB*)FastSPI_LED.getRGBData(); // grab out the data thing
    }

    bool set_light(unsigned int idx, byte r, byte g, byte b) 
    {
        //debug("set_light()");
        //debug(idx);
        if (idx > __TOTAL_LIGHT_COUNT__)
        {
            debug("Bad light -- SetLight");
            debug(idx);
            return false;
        }
        //int idx = get_light_index(row, column);
        leds[idx].r = r;
        leds[idx].g = g;
        leds[idx].b = b;
        return true;
    }

    void show() 
    {
       FastSPI_LED.show();
    }

    void show(int strand) 
    {
       FastSPI_LED.show(strand);
    }

    void clear() 
    {
       memset(leds, 0, (__TOTAL_LIGHT_COUNT__) * 3);
    }

    int get_strands() 
    {
        return __NUM_STRANDS__;
    }

    int get_total_light_count()
    {
        return __TOTAL_LIGHT_COUNT__;
    }

    void debug_loop()
    {
        debug("lights_via_fastspi.h GridLightController debug_loop()");
        for(int j = 0; j < 3; j++) {   
            for(int i = 0 ; i < __TOTAL_LIGHT_COUNT__; i++ ) {
                memset(leds, 0, (__TOTAL_LIGHT_COUNT__) * 3);
                switch(j) { 
                    case 0: leds[i].r = 255; break;
                    case 1: leds[i].g = 255; break;
                    case 2: leds[i].b = 255; break;
                }
                FastSPI_LED.show();
                delay(10);
            }
        }
        debug("END lights_via_fastspi.h GridLightController debug_loop()");   
    }
};



