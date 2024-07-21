// Core library for code-sense


#include <FastLED.h>
#include <Arduino_DebugUtils.h>

#ifndef DATA_PIN
//#define DATA_PIN 18 // esp32 c6
#define DATA_PIN 29 // waveshare strip
//#define DATA_PIN 16 // ?? waveshare built-in?
#endif

#ifndef NUM_LEDS
#define NUM_LEDS 50

#endif

#ifndef __LIGHT_CONFIGURATION_DEFINES__
#define __LIGHT_CONFIGURATION_DEFINES__
#define __ORIENTATION__ 0x01 // That is, linear
#define __NUM_STRANDS__ 1
#define __LIGHT_PINS__ {0}//, 4, 6}
#define __LIGHTS_AT_PINS__ {NUM_LEDS}//, 17, 8}
#define __TOTAL_LIGHTS__ NUM_LEDS
#define __BRIGHTNESS_SCALE__ 1.0
#endif

class GridLightControl 
{
private:
    CRGB leds[NUM_LEDS];
    float BrightnessScale;

    enum 
    {
        LINEAR_ORIENTATION = 0x01,
        ZIGZAG_ORIENTATION = 0x02
    };
    
    int totallightcount;

public:
    // constructor
    GridLightControl() 
    {
        totallightcount = __TOTAL_LIGHTS__;
        BrightnessScale = __BRIGHTNESS_SCALE__;
    }

    void init() 
    {
      FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    }
    
    void brighten()
    {
      //BrightnessScale = BrightnessScale + 0.1 >= 1 ? 1 : BrightnessScale + 0.1;

      if (BrightnessScale > 0.09) {
        DEBUG_INFO("PUSH WHOLE STEP, %f to", BrightnessScale);
        BrightnessScale = BrightnessScale + 0.1 >= 1 ? 1 : BrightnessScale + 0.1;
      } else {
        DEBUG_INFO("PUSH 1/10th STEP, %f to", BrightnessScale);
        BrightnessScale = BrightnessScale + 0.01 >= 1 ? 1 : BrightnessScale + 0.01;
      }
      DEBUG_INFO("%f", BrightnessScale);
    }

    void dim()
    {
      if (BrightnessScale <= 0.1)
        BrightnessScale = BrightnessScale - 0.01 <= 0 ? 0 : BrightnessScale - 0.01;
      else
        BrightnessScale = BrightnessScale - 0.1 <= 0 ? 0 : BrightnessScale - 0.1;
      DEBUG_INFO("%f", BrightnessScale);
    }

    bool set_light(unsigned int idx, byte r, byte g, byte b) 
    {
        if (idx > totallightcount)
        {
            return false;
        }
        
        leds[idx].r = r * BrightnessScale;
        leds[idx].g = g * BrightnessScale;
        leds[idx].b = b * BrightnessScale;
        
        return true;
    }
    
    bool set_light__fade_intermediary(unsigned int idx, byte r, byte g, byte b)
    {
        if (idx > totallightcount)
        {
            return false;
        }

        uint8_t rscaled = r * BrightnessScale;
        uint8_t gscaled = g * BrightnessScale;
        uint8_t bscaled = b * BrightnessScale;
        
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
        DEBUG_VERBOSE("[LightController.set_light__fade_intermediary] called for Index: %d, RGB: %x, %x, %x, OldRGB: %x, %x, %x, Fraction: %f", idx, r, g, b, rold, gold, bold, frac);

        if (idx > totallightcount)
        {
            DEBUG_VERBOSE("[LightController.set_light__fade_intermediary] called, short-circuit because index %d > totallightcount %d", idx, totallightcount );
            return false;
        }
        
        uint8_t rscaled = r * BrightnessScale;
        uint8_t gscaled = g * BrightnessScale;
        uint8_t bscaled = b * BrightnessScale;
        
        uint8_t rold_scaled = rold * BrightnessScale;
        uint8_t gold_scaled = gold * BrightnessScale;
        uint8_t bold_scaled = bold * BrightnessScale;
        
        leds[idx].r = (rscaled * frac) + (rold_scaled * (1 - frac));
        leds[idx].g = (gscaled * frac) + (gold_scaled * (1 - frac));
        leds[idx].b = (bscaled * frac) + (bold_scaled * (1 - frac));
        
        return true;
    }

    void show()
    {
      DEBUG_VERBOSE("[LightController.show] called");
        FastLED.show();
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
                FastLED.show();
                delay(100);
            }
        }
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
                FastLED.show();
                //FastSPI_LED.show();
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
                FastLED.show();
                //FastSPI_LED.show();
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
            FastLED.show();
            delay(3);
        }
        for (int j = 0; j < 2; j++)
        {
            rold = r;
            gold = g;
            bold = b;
            r = (uint8_t)random(0, 255);
            g = (uint8_t)random(0, 255);
            b = (uint8_t)random(0, 255);
            
            for (int kup = 0, kdn = 255; kup < 256; kup++, kdn--)
            {
                for(int i = 0; i < totallightcount; i++ ) {                    
                    leds[i].r = (r * ((double)kup/256.0)) + (rold * ((double)kdn/255.0));
                    leds[i].g = (g * ((double)kup/256.0)) + (gold * ((double)kdn/255.0));
                    leds[i].b = (b * ((double)kup/256.0)) + (bold * ((double)kdn/255.0));
                }
                FastLED.show();
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
            FastLED.show();
            delay(3);
        }
    }
};


