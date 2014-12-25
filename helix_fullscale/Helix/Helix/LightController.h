// Core library for code-sense
#include "IDEIncludes.h"

#ifndef LIGHTS_VIA_FASTSPI_H
#define LIGHTS_VIA_FASTSPI_H

#include "ConfigurationDefines.h"
#include "Debug.h"

#include "WS2812_Bitbang.h"



class GridLightControl 
{
private:
    WS2812_Bitbang FastSPI_LED;
    struct CRGB { unsigned char g; unsigned char r; unsigned char b; }; // for whatever reason, the order is wrong here.
    struct CRGB * leds;

    enum 
    {
        LINEAR_ORIENTATION = 0x01,
        ZIGZAG_ORIENTATION = 0x02
    };
    
    int totallightcount;
public:
    int lightsatpins[__NUM_STRANDS__];
    
public:
    // constructor
    GridLightControl() 
    {
        totallightcount = 0;
        int tmplightsatpins[__NUM_STRANDS__] = __LIGHTS_AT_PINS__;
        
        for (int i = 0; i < __NUM_STRANDS__; i++)
        {
            lightsatpins[i] = tmplightsatpins[i];
            totallightcount += lightsatpins[i];
        }
    }

    void init() 
    {
        FastSPI_LED.begin();
        leds = (struct CRGB*)FastSPI_LED.getRGBData(); // grab out the data thing
        Debug::debugln("Light Init Done");
    }
    
    bool set_light(unsigned int idx, byte r, byte g, byte b) 
    {
        if (idx > totallightcount)
        {
            return false;
        }
        leds[idx].r = r * __BRIGHTNESS_SCALE__;
        leds[idx].g = g * __BRIGHTNESS_SCALE__;
        leds[idx].b = b * __BRIGHTNESS_SCALE__;
        /*
        if (leds[idx].r > 62 || leds[idx].g > 62 || leds[idx].b > 62 )
        {
            Debug::debug(leds[idx].r);
            Debug::debug("-");
            Debug::debug(leds[idx].g);
            Debug::debug("-");
            Debug::debugln(leds[idx].b);
            
        } */
        
        return true;
    }
    
    bool set_light__fade_intermediary(unsigned int idx, byte r, byte g, byte b)
    {
        if (idx > totallightcount)
        {
            return false;
        }

        uint8_t rscaled = r * __BRIGHTNESS_SCALE__;
        uint8_t gscaled = g * __BRIGHTNESS_SCALE__;
        uint8_t bscaled = b * __BRIGHTNESS_SCALE__;
        
        uint8_t rold = leds[idx].r;
        uint8_t gold = leds[idx].g;
        uint8_t bold = leds[idx].b;
        
        leds[idx].r = (rscaled * .5) + (rold * .5);
        leds[idx].g = (gscaled * .5) + (gold * .5);
        leds[idx].b = (bscaled * .5) + (bold * .5);

        return true;
    }
    
    bool set_light__fade_intermediary(unsigned int idx, byte r, byte g, byte b, byte rold, byte gold, byte bold, double frac)
    {
        if (idx > totallightcount)
        {
            return false;
        }
        
        uint8_t rscaled = r * __BRIGHTNESS_SCALE__;
        uint8_t gscaled = g * __BRIGHTNESS_SCALE__;
        uint8_t bscaled = b * __BRIGHTNESS_SCALE__;
        
        uint8_t rold_scaled = rold * __BRIGHTNESS_SCALE__;
        uint8_t gold_scaled = gold * __BRIGHTNESS_SCALE__;
        uint8_t bold_scaled = bold * __BRIGHTNESS_SCALE__;
        
        //        uint8_t rold = leds[idx].r;
        //        uint8_t gold = leds[idx].g;
        //        uint8_t bold = leds[idx].b;
        
        leds[idx].r = (rscaled * frac) + (rold_scaled * (1 - frac));
        leds[idx].g = (gscaled * frac) + (gold_scaled * (1 - frac));
        leds[idx].b = (bscaled * frac) + (bold_scaled * (1 - frac));
        
        return true;
    }

    void show()
    {
        
        FastSPI_LED.show();
        //FastSPI_LED.show_parallel();
    }

    void clear() 
    {
       memset(leds, 0, (totallightcount) * 3);
    }

    int get_strands() 
    {
        return __NUM_STRANDS__;
    }

    int get_total_light_count()
    {
        return totallightcount;
    }

    void debug_loop()
    {
        //debug("lights_via_fastspi.h GridLightController debug_loop()");
        for(int j = 0; j < 3; j++) {   
            for(int i = 0 ; i < totallightcount; i++ ) {
                memset(leds, 0, (totallightcount) * 3);
                switch(j) { 
                    case 0: leds[i].r = 255; break;
                    case 1: leds[i].g = 255; break;
                    case 2: leds[i].b = 255; break;
                }
                FastSPI_LED.show();
                delay(100);
            }
        }
        //debug("END lights_via_fastspi.h GridLightController debug_loop()");
    }
    
    void debug_loop_fade()
    {
        // Fade in/fade out
        for(int j = 0; j < 3; j++ ) {
            memset(leds, 0, totallightcount * 3);
            for(int k = 0; k < 256; k++) {
                for(int i = 0; i < totallightcount; i++ ) {
                    switch(j) {
                        case 0: leds[i].r = k; break;
                        case 1: leds[i].g = k; break;
                        case 2: leds[i].b = k; break;
                    }
                }
                FastSPI_LED.show();
                delay(3);
            }
            for(int k = 255; k >= 0; k--) {
                for(int i = 0; i < totallightcount; i++ ) {
                    switch(j) {
                        case 0: leds[i].r = k; break;
                        case 1: leds[i].g = k; break;
                        case 2: leds[i].b = k; break;
                    }
                }
                FastSPI_LED.show();
                delay(3);
            }
        }
    }
    
    void debug_loop_randomfade()
    {
        // Fade in/fade out
        memset(leds, 0, totallightcount * 3);
        
        int r, g, b, rold, gold, bold;
        
        r = (uint8_t)random(0, 255);
        g = (uint8_t)random(0, 255);
        b = (uint8_t)random(0, 255);
        for(int k = 0; k < 256; k++) {
            for(int i = 0; i < totallightcount; i++ ) {
                
                //intrval = r * ((double)k/256.0)
                
                leds[i].r = r * ((double)k/256.0);
                leds[i].g = g * ((double)k/256.0);
                leds[i].b = b * ((double)k/256.0);
            }
            FastSPI_LED.show();
            delay(3);
        }
        for (int j = 0; j < 30; j++)
        {
            rold = r;
            gold = g;
            bold = b;
            r = (uint8_t)random(0, 255);
            g = (uint8_t)random(0, 255);
            b = (uint8_t)random(0, 255);
            
            //Serial.println(String(r) + " " + String(g) + " " + String(b));
            /*
            Debug::debug(r);
            Debug::debug(" ");
            Debug::debug(g);
            Debug::debug(" ");
            Debug::debugln(b);
            */

            for (int kup = 0, kdn = 255; kup < 256; kup++, kdn--)
            {
                for(int i = 0; i < totallightcount; i++ ) {                    
                    leds[i].r = (r * ((double)kup/256.0)) + (rold * ((double)kdn/255.0));
                    leds[i].g = (g * ((double)kup/256.0)) + (gold * ((double)kdn/255.0));
                    leds[i].b = (b * ((double)kup/256.0)) + (bold * ((double)kdn/255.0));
                }
                FastSPI_LED.show();
                delay(3);
            }
        }
        // fade out
        for(int k = 255; k >= 0; k--) {
            for(int i = 0; i < totallightcount; i++ ) {
                leds[i].r = r * ((double)k/255.0);
                leds[i].g = g * ((double)k/255.0);
                leds[i].b = b * ((double)k/255.0);
            }
            FastSPI_LED.show();
            delay(3);
        }
    }
};



#endif