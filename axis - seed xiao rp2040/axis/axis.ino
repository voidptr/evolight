#define DECODE_NEC

#include <Arduino.h>

// debugging lib
#include <Arduino_DebugUtils.h>

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library

// light control lib
#include <FastLED.h>

// helix-axis
#include "LightController.h"
#include "GA.h"



GA Engine;
GridLightControl Lights;

void setup()
{
  Serial.begin(115200);
  Debug.timestampOn();
  Debug.setDebugLevel(DBG_INFO);

    // Just to know which program is running on my Arduino
  DEBUG_INFO("START %s from %s\r\nUsing library version", __FILE__, __DATE__, VERSION_IRREMOTE);

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  DEBUG_INFO("Ready to receive IR signals of protocols: ");
  printActiveIRProtocols(&Serial);
  DEBUG_INFO("at pin %d", IR_RECEIVE_PIN);
  
  Engine.init();
  Lights.init();
}
int i = 0;
void loop()
{

  // serial
  if (IrReceiver.decode()) {

      /*
        * Print a summary of received data
        */
      if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
          Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
          // We have an unknown protocol here, print extended info
          IrReceiver.printIRResultRawFormatted(&Serial, true);
          IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
      } else {
          IrReceiver.resume(); // Early enable receiving of the next IR frame
          IrReceiver.printIRResultShort(&Serial);
          IrReceiver.printIRSendUsage(&Serial);
      }
      Serial.println();

      /*
        * Finally, check the received data and perform actions according to the received command
        */
      if (IrReceiver.decodedIRData.command == 0x10) {
          // do something
      } else if (IrReceiver.decodedIRData.command == 0x11) {
          // do something else
      }
  }

  // evo
  DEBUG_INFO("i = %d", i);
  i++;

  Engine.evolve();
  output_lights();

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


