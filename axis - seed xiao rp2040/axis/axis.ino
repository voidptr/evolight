#define DECODE_NEC

#include <Arduino.h>

// debugging lib
#include <Arduino_DebugUtils.h>

// Circular Buffer
#include <CircularBuffer.hpp>

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

#define FADE_FRACTION 100

enum Commands {
  BrightnessIncrease, // 0x00
  BrightnessDecrease, // 0x01
  Off, // 0x02
  On, // 0x03
  AddRed, // 0x04
  AddGreen, // 0x05
  AddBlue, // 0x06
  AddWhite, // 0x07 -- (W) white
  AddOrangeRed, // 0x08
  AddLeafGreen, // 0x09
  AddMediumBlue, // 0x0A
  IncreaseMutationRate, // 0x0B -- (FLASH) 
  AddOrange, // 0x0C
  AddSkyBlue, // 0x0D
  AddPurple, // 0x0E
  SmoothMode, // 0x0F -- Top organism is displayed only
  AddKhaki, // 0x10
  AddTurquoise, // 0x11
  AddViolet, // 0x12
  FadeMode, // 0x13 -- Move the top organism along the light string 
  AddYellow, // 0x14
  AddDarkBlue, // 0x15
  AddMagenta, // 0x16
  StrobeMode, // 0x17 -- Default (each organism cycled across a single pixel)
};

CircularBuffer<int, 100> IrCommands;

GA Engine;
GridLightControl Lights;
int i = 0;
int fade_fraction = FADE_FRACTION;

#define INDICATOR_COUNT 10
#define INDICATOR_PIN 15

CRGB indicator_led[INDICATOR_COUNT]; // built-in LED 

void setup()
{
  Serial.begin(115200);
  Debug.timestampOn();
  Debug.setDebugLevel(DBG_INFO);

    // Just to know which program is running on my Arduino
  DEBUG_INFO("START %s from %s\r\nUsing library version", __FILE__, __DATE__, VERSION_IRREMOTE);
  
  Engine.init();
  Lights.init();
}

void loop()
{
  // evo
  DEBUG_INFO("i = %d", i);
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

void setup1()
{

    // Just to know which program is running on my Arduino
  DEBUG_INFO("START %s from %s\r\nUsing library version", __FILE__, __DATE__, VERSION_IRREMOTE);

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  DEBUG_INFO("Ready to receive IR signals of protocols: ");
  printActiveIRProtocols(&Serial);
  DEBUG_INFO("at pin %d", IR_RECEIVE_PIN);

  pinMode(14, OUTPUT); // 5v to the IR reader power pin
  digitalWrite(14, HIGH); // 5v to the IR reader power pin

  FastLED.addLeds<WS2812, INDICATOR_PIN>(indicator_led, INDICATOR_COUNT); // single WS2812 built-in LED (waveshare pin 16) 
}

void loop1()
{
    // serial
    if (IrReceiver.decode()) 
    {
        /*
          * Print a summary of received data
          */
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            //Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print extended info
            //IrReceiver.printIRResultRawFormatted(&Serial, true);
            IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
        } else {
            IrReceiver.resume(); // Early enable receiving of the next IR frame
            //IrReceiver.printIRResultShort(&Serial);
            //IrReceiver.printIRSendUsage(&Serial);

            if (!IrCommands.push(IrReceiver.decodedIRData.command))
            {
                DEBUG_ERROR("Overflowed circular buffer. Erasing head");
            }
            else
            {
                DEBUG_VERBOSE("Collected command %d", IrReceiver.decodedIRData.command);
            }
        }
    }
 }

void increase_fade()
{
    DEBUG_INFO("Fade Fraction %d", fade_fraction);
    fade_fraction += 10;
    DEBUG_INFO("Fade Increase %d", fade_fraction);
}

void decrease_fade()
{
    fade_fraction -= 10;
    if (fade_fraction <= 0)
      fade_fraction = 1;

    DEBUG_INFO("Fade Decrease %d", fade_fraction);
}

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

void CheckCommands()
{
    if (IrCommands.isEmpty())
    {
        DEBUG_VERBOSE("No commands to process");
    }
    else
    {
        while (!IrCommands.isEmpty())
        {
          int command = IrCommands.pop();
          Commands cmd = static_cast<Commands>(command);
          //DEBUG_INFO("Received Command: %d, %s", command, cmd);

          ActionCommand(cmd);

        }
    }
}

void ActionIndicator(byte r, byte g, byte b, int pauselength=100)
{
    DEBUG_VERBOSE("[ActionIndicator] called");
    indicator_led[0].r = r;
    indicator_led[0].g = g;
    indicator_led[0].b = b;
    FastLED.show();
}

void ActionCommand(Commands cmd)
{
    switch (cmd)
    {
        case BrightnessIncrease: // 0x00
            DEBUG_INFO("Increase Brightness");
            ActionIndicator(255, 255, 255);
            Lights.brighten();
            ActionIndicator(0, 0, 0, 3);
            break;
        case BrightnessDecrease: // 0x01
            DEBUG_INFO("Decrease Brightness");
            ActionIndicator(128, 128, 128);
            Lights.dim();
            ActionIndicator(0, 0, 0, 3);
            break;
        case Off: // 0x02
            DEBUG_INFO("Off - Reset GA");
            ActionIndicator(0, 255, 0);
            Engine.reset_ga();
            ActionIndicator(0, 0, 0, 3);
            fade_fraction = FADE_FRACTION;
            break;
        case On: // 0x03
            DEBUG_INFO("On - Randomize Population");
            ActionIndicator(255, 0, 0);
            ActionIndicator(0, 255, 0);
            ActionIndicator(0, 0, 255);
            Engine.randomize_population(0);
            ActionIndicator(0, 0, 0, 3);
            break;
        case AddRed: // 0x04
            DEBUG_INFO("AddRed - bias mutation rate toward red");
            ActionIndicator(255, 0, 0);
            Engine.increase_bias(true, false, false);
            ActionIndicator(0, 0, 0, 3);
            break;
        case AddGreen: // 0x05
            DEBUG_INFO("AddGreen");
            ActionIndicator(0, 255, 0);
            Engine.increase_bias(false, true, false);
            ActionIndicator(0, 0, 0, 3);
            break;  
        case AddBlue: // 0x06
            DEBUG_INFO("AddBlue");
            ActionIndicator(0, 0, 255);
            Engine.increase_bias(false, false, true);
            ActionIndicator(0, 0, 0, 3);
            break;  
        case AddWhite: // 0x07 -- (W) white
            DEBUG_INFO("AddWhite");
            Engine.increase_bias(true, true, true);
            break;
        case AddOrangeRed: // 0x08
            DEBUG_INFO("AddOrangeRed");
            Engine.increase_bias(true, false, false); // red
            Engine.increase_bias(true, false, false); // red
            Engine.increase_bias(true, true, false); // red+green = yellow
            break;
        case AddLeafGreen: // 0x09
            DEBUG_INFO("AddLeafGreen");
            Engine.increase_bias(false, true, false); // green
            Engine.increase_bias(true, true, false); // red+green = yellow
            break;
        case AddMediumBlue: // 0x0A
            DEBUG_INFO("AddMediumBlue");
            Engine.increase_bias(false, false, true); // blue
            Engine.increase_bias(true, true, true); // lighten
            break;
        case IncreaseMutationRate: // 0x0B -- (FLASH) 
            DEBUG_INFO("FlashMode - IncreaseMutationRate");
            ActionIndicator(255, 0, 0);
            ActionIndicator(0, 0, 255);
            Engine.increase_mutation_rate();
            ActionIndicator(0, 0, 0, 3);
            break;
        case AddOrange: // 0x0C
            DEBUG_INFO("AddOrange");
            Engine.increase_bias(true, false, false); // red
            Engine.increase_bias(true, true, false); // red+green = yellow
            break;
        case AddSkyBlue: // 0x0D
            DEBUG_INFO("AddSkyBlue");
            Engine.increase_bias(false, false, true); // blue
            Engine.increase_bias(true, true, true); // lighten
            Engine.increase_bias(true, true, true); // lighten
            break;
        case AddPurple: // 0x0E
            DEBUG_INFO("AddPurple");
            Engine.increase_bias(true, false, true); // red+blue = purple
            break;
        case SmoothMode: // 0x0F -- Do a fast evolve and get through the rough patch
            DEBUG_INFO("SmoothMode - Evolving 100 generations");
            ActionIndicator(0, 255, 255);
            evolve_steps(100);
            ActionIndicator(0, 0, 0, 3);
            break;
        case AddKhaki: // 0x10
            DEBUG_INFO("AddKhaki");
            Engine.increase_bias(true, false, false); // red
            Engine.increase_bias(true, true, false); // red+green = yellow
            Engine.increase_bias(true, true, false); // red+green = yellow
            break;
        case AddTurquoise: // 0x11
            DEBUG_INFO("AddTurquoise");
            Engine.increase_bias(false, true, true); // blue+green
            break;
        case AddViolet: // 0x12
            DEBUG_INFO("AddViolet");
            Engine.increase_bias(true, false, true); // red+blue = purple
            Engine.increase_bias(true, false, false); // red
            break;
        case FadeMode: // 0x13  
            DEBUG_INFO("FadeMode");
            ActionIndicator(0, 255, 255);
            increase_fade();
            ActionIndicator(0, 0, 0, 3);
            break;
        case AddYellow: // 0x14
            DEBUG_INFO("AddYellow");
            Engine.increase_bias(true, true, false); // red+green = yellow
            break;
        case AddDarkBlue: // 0x15
            DEBUG_INFO("AddDarkBlue");
            Engine.increase_bias(false, false, true); // red+green = yellow
            break;
        case AddMagenta: // 0x16
            DEBUG_INFO("AddMagenta");
            Engine.increase_bias(true, false, true); // red+blue = purple
            Engine.increase_bias(true, false, false); // red
            Engine.increase_bias(true, false, false); // red
            break;
        case StrobeMode: // 0x17 -- Default (each organism cycled across a single pixel)
            DEBUG_INFO("StrobeMode");
            ActionIndicator(255, 0, 0);
            decrease_fade();
            ActionIndicator(0, 0, 0, 3);
            break;
        default:
            DEBUG_INFO("Default case");
    }
}


