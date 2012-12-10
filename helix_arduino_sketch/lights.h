#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif


#define __NUM_STRANDS__ 2
#define __LIGHTS_PER_STRAND__ 120
#define __ORIENTATION__ 0x01 // That is, linear
#define __NUM_ROWS__ 10
#define __NUM_COLUMNS__ 30

class LightsHardware 
{   
private:
//    CFastSPI_LED * &FastSPI_LED;
    struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
    struct CRGB * leds;

    int mPin;
    int mCount;

public:
    LightsHardware(int pin, int count) 
    {
        debug("LightsHardware CTOR");
        mPin = pin;
        mCount = count;
        debug("END LightsHardware CTOR");
    }

    void init()
    {
        debug("LightsHardware init()");

        leds = (struct CRGB*)FastSPI_LED.getRGBData(); // grab out the data thing

        debug("END LightsHardware init()");
    }

    void start() {
        FastSPI_LED.setLeds(mCount);
        FastSPI_LED.setPin(0, mPin, mCount); // this version DOES NOT do a malloc, so no memory leak
        FastSPI_LED.init();
        FastSPI_LED.start();
    }

    void stop() {
        FastSPI_LED.stop();
    }


    void set_light(int index, byte r, byte g, byte b) 
    {
        leds[index].r = r;
        leds[index].g = g;
        leds[index].b = b;
    }

    void show() 
    {
        FastSPI_LED.show();
    }
};


class GridLightControl 
{
private:
    LightsHardware * Strands[__NUM_STRANDS__];    
    int mCurrentStrand;
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
        Strands[0] = new LightsHardware(4, __LIGHTS_PER_STRAND__);
        Strands[1] = new LightsHardware(5, __LIGHTS_PER_STRAND__);
        debug("END GridLightControl CTOR");

        mCurrentStrand = -1
    }

    void init() 
    {
        FastSPI_LED.setPinCount(1); // make it do the malloc thing.
        for (int i = 0; i < __NUM_STRANDS__; i++)
        {
            Strands[i]->init();
        }
    }

    void set_light(int row, int column, byte r, byte g, byte b) 
    {
        int idx = get_light_index(row, column);
        int strand = get_strand(idx);
        int strand_idx = get_strand_index(idx);   

        switch_strand(strand); // if needed.
        Strands[strand]->set_light(strand_idx, r, g, b);
    }

    void switch_strand(int strand)
    {
        if (strand != mCurrentStrand)
        {
            if (mCurrentStrand != -1)
            {
                Strands[mCurrentStrand]->show(); // push the thing
                Strands[mCurrentStrand]->stop()
            }
            if (strand != -1)
            {
                Strands[strand]->start();
            }
            mCurrentStrand = strand;
        }
    }

    void show() 
    {
        allocate_to_strand(-1); // reset to none, close up shop.
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

        Strands[0]->debug_loop();
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

    int get_strand(int index) {
        return index / __LIGHTS_PER_STRAND__;
    }
    int get_strand_index(int index) {
        return index % __LIGHTS_PER_STRAND__;
    }

};



