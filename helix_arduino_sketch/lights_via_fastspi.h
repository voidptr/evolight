#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif


#define __TOTAL_LIGHT_COUNT__ 120


#define __ORIENTATION__ 0x01 // That is, linear
#define __NUM_ROWS__ 10
#define __NUM_COLUMNS__ 12

class GridLightControl 
{
private:
    struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
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
        debug("GridLightControl CTOR");  
        // THIS BIT HERE IS SUPER GHETTO -
        // --- UPDATE THIS IF YOU ADD MORE STRANDS ZOMG ---
        debug("END GridLightControl CTOR");

    }

    void init() 
    {
        FastSPI_LED.setLeds(__TOTAL_LIGHT_COUNT__);
        FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2811);

        FastSPI_LED.setPinCount(5); 
        FastSPI_LED.setPin(0, 4, 24); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(1, 5, 24); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(2, 6, 24); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(3, 7, 24); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.setPin(4, 8, 24); // this version DOES NOT do a malloc, so no memory leak


        FastSPI_LED.init();
        FastSPI_LED.start();
        leds = (struct CRGB*)FastSPI_LED.getRGBData(); // grab out the data thing
    }

    void set_light(int row, int column, byte r, byte g, byte b) 
    {
        int idx = get_light_index(row, column);
        leds[idx].r = r;
        leds[idx].g = g;
        leds[idx].b = b;
    }

    void show() 
    {
       FastSPI_LED.show();
    }

    void clear() 
    {
       memset(leds, 0, (__TOTAL_LIGHT_COUNT__) * 3);
       //show();
       //delay(100);
    }


    int get_rows() 
    {
        return __NUM_ROWS__;
    }

    int get_columns() 
    {
        return __NUM_COLUMNS__;
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

private:
    int get_light_index(int row, int column) 
    { // treats all strands as contiguous     
        if ((__ORIENTATION__ == GridLightControl::LINEAR_ORIENTATION) || 
            (column % 2) > 0) // LINEAR or Even Column
        {
            return (row * __NUM_COLUMNS__) + column;
        }
        else // ZIGZAG and odd column
        {
            return (row * __NUM_COLUMNS__) + (__NUM_COLUMNS__ - column);
        }
    }
};



