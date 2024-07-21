#include <Arduino_DebugUtils.h>
#include <FastLED.h>

#include "LightController.h"
#include "GA.h"

GA Engine;
GridLightControl Lights;

void setup()
{
  Serial.begin(115200);
  Debug.timestampOn();
  Debug.setDebugLevel(DBG_INFO);


  //delay(3000);
  //DEBUG_INFO("High precision timing measurements/delays can be built using");
  //DEBUG_INFO("rp2040.getCycleCount() and rp2040.getCycleCount64().\n");
  //uint32_t a = rp2040.getCycleCount();
  //delay(1000);
  //uint32_t b = rp2040.getCycleCount();
  //DEBUG_INFO("There are %lu cycles in one second.\n\n\n", b - a);

  //delay(3000);

  //DEBUG_INFO("Note that the (32-bit) getCycleCount() will wraparound in ~30 seconds.");
  //DEBUG_INFO("Using the 64-bit getCycleCount64() makes it practically infinite.");
  //DEBUG_INFO("\n%15s - %15s\n", "getCycleCount", "getCycleCount64");

  //pinMode(LED, OUTPUT);
  
  Engine.init();
  Lights.init();
}
int i = 0;
void loop()
{
  DEBUG_INFO("i = %d", i);
  i++;
  //delay(1000);              // wait for a second
  
  
  //Engine.evolve();
  //Engine.debug_all();
  //output_lights();
  Lights.debug_loop_randomfade();

}

#define FADE_FRACTION 100
void output_lights()
{
    for (int seq = 0; seq < __LOCUS_COUNT__; seq++)
    {
        DEBUG_DEBUG("seq %d of %d", seq, __LOCUS_COUNT__);
        for (int frac = 0; frac < FADE_FRACTION; frac++)
        {
            DEBUG_DEBUG("frac %d of %d", frac, FADE_FRACTION);
            double fracval = (double)frac/FADE_FRACTION;
            for (int org = 0; org < __TOTAL_ORGANISMS__; org++)
            {
                DEBUG_DEBUG("org %d of %d", org, __TOTAL_ORGANISMS__);
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


