#define DECODE_NEC

#include <Arduino.h>

// debugging lib
#include <Arduino_DebugUtils.h>
#include <Adafruit_NeoPixel.h>

// helix-axis
#include "GA.h"

#define FADE_FRACTION 100
#define __BRIGHTNESS_SCALE__ 1.0

#define LED_PIN 13 // ESP32S3 Supermini external
//#define LED_PIN 48 // ESP32S3 Supermini built-in
//#define LED_PIN D10 // ESP32C6
//#define LED_PIN 10 // SAMD21
//#define LED_PIN 29 // Waveshare?
//#define LED_PIN 16 // Waveshare Matrix
#define LED_COUNT 50
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

GA Engine;
int i = 0;
int fade_fraction = FADE_FRACTION;
float BrightnessScale = __BRIGHTNESS_SCALE__;



int threshold = 40;
bool touchActive = false;
bool lastTouchActive = false;
bool testingLower = true;

void gotTouchEvent() {
  if (lastTouchActive != testingLower) {
    touchActive = !touchActive;
    testingLower = !testingLower;
    // Touch ISR will be inverted: Lower <--> Higher than the Threshold after ISR event is noticed
    //touchInterruptSetThresholdDirection(testingLower);
  }
}

void CheckCommands() {
    if (lastTouchActive != touchActive) {
        lastTouchActive = touchActive;
        if (touchActive) {
            Serial.println("  ---- Touch was Pressed");
        } else {
            Serial.println("  ---- Touch was Released");
        }
        Serial.printf("T2 pin2 = %d \n", touchRead(T8));
    }
    
}


void setup()
{
    // TO ENABLE SERIAL OUTPUT
    // set up IDE with
    // (a) Board ESP32S3 Dev Module
    // (b) "USB CDC on boot : enabled"
    // (c) JTAG adapter disabled

    Serial.begin(115200);
    Debug.timestampOn();
    Debug.setDebugLevel(DBG_INFO);
 
    while ( !Serial && millis() < 10000 ) {
        delay(500) ;
    }

    Serial.printf("Begin\n") ;

    Serial.println("ESP32 Touch Interrupt Test");
    touchAttachInterrupt(T8, gotTouchEvent, threshold);

    // Touch ISR will be activated when touchRead is lower than the Threshold
    //touchInterruptSetThresholdDirection(testingLower);

    DEBUG_INFO("STARTING INIT");
        // Just to know which program is running on my Arduino
    //DEBUG_INFO("START %s from %s\r\nUsing library version", __FILE__, __DATE__, VERSION_IRREMOTE);

    Engine.init();

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(255 * BrightnessScale);
}

void loop()
{


    Serial.printf("Generation i = %d\n", i) ;
    // // evo
    // DEBUG_INFO("i = %d", i);
    i++;

    Engine.evolve();
    output_lights();

}

void evolve_steps(int steps)
{
    DEBUG_INFO("Evolving %d steps", steps);
    for (int j = 0; j < steps; j++)
    {
        Engine.evolve();
    }
    i += steps;
}

// void increase_fade()
// {
//     DEBUG_INFO("Fade Fraction %d", fade_fraction);
//     fade_fraction += 10;
//     DEBUG_INFO("Fade Increase %d", fade_fraction);
// }

// void decrease_fade()
// {
//     fade_fraction -= 10;
//     if (fade_fraction <= 0)
//       fade_fraction = 1;

//     DEBUG_INFO("Fade Decrease %d", fade_fraction);
// }

void output_lights()
{
    for (int seq = 0; seq < __LOCUS_COUNT__; seq++)
    {
        DEBUG_DEBUG("seq %d of %d", seq, __LOCUS_COUNT__);
        for (int frac = 0; frac < fade_fraction; frac++)
        {
            DEBUG_DEBUG("frac %d of %d", frac, fade_fraction);
            double fracval = (double)frac/fade_fraction;
            for (int org = 0; org < __TOTAL_ORGANISMS__; org++)
            {
                CheckCommands();

                DEBUG_DEBUG("org %d of %d", org, __TOTAL_ORGANISMS__);
                int traitprev = ((seq + org) % __LOCUS_COUNT__);
                int trait = ((seq + org + 1) % __LOCUS_COUNT__);

                strip.setPixelColor(
                    org, // index
                    strip.Color(
                        fade(Engine.Organisms[org].Traits[trait].r, Engine.Organisms[org].Traits[traitprev].r, fracval), 
                        fade(Engine.Organisms[org].Traits[trait].g, Engine.Organisms[org].Traits[traitprev].g, fracval), 
                        fade(Engine.Organisms[org].Traits[trait].b, Engine.Organisms[org].Traits[traitprev].b, fracval)));

            }
            //Lights.show();
            strip.show();
            delay(3);
        }
    }
}

uint8_t fade(byte c, byte cold, double frac) {
    uint8_t cscaled = c * BrightnessScale;
    uint8_t cold_scaled = cold * BrightnessScale;
    uint8_t final_c = (cscaled * frac) + (cold_scaled * (1 - frac));
    return final_c;
}


