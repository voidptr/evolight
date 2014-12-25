#include <FastSPI_LED_Teensy3.h>

#include "LightController.h"
#include "GA.h"

#define LED 13
#define NUM_LEDS 32

GA Engine;
GridLightControl Lights;

void setup()
{
  pinMode(LED, OUTPUT);
  
  Engine.init();
  Lights.init();
}

void loop()
{
  //digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(500);               // wait for a second
  //digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  //delay(100);               // wait for a second
  
  Engine.evolve();
  output_lights();

}

#define FADE_FRACTION 100
void output_lights()
{
    for (int seq = 0; seq < __LOCUS_COUNT__; seq++)
    {
        for (int frac = 0; frac < FADE_FRACTION; frac++)
        {
            double fracval = (double)frac/FADE_FRACTION;
            for (int org = 0; org < __TOTAL_ORGANISMS__; org++)
            {
                int traitprev = ((seq + org) % __LOCUS_COUNT__);
                int trait = ((seq + org + 1) % __LOCUS_COUNT__);
                bool lightok = Lights.set_light__fade_intermediary(org,
                                                    Engine.Organisms[org].Traits[trait].r,
                                                    Engine.Organisms[org].Traits[trait].g,
                                                    Engine.Organisms[org].Traits[trait].b,
                                                    Engine.Organisms[org].Traits[traitprev].r,
                                                    Engine.Organisms[org].Traits[traitprev].g,
                                                    Engine.Organisms[org].Traits[traitprev].b,
                                                    fracval);
                if (!lightok)
                {
                    break;
                }
            }
            Lights.show();
            delay(3);
        }

    }
}


