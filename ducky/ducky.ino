#include <Arduino.h>

// debugging lib
#include <Arduino_DebugUtils.h>
#include <Adafruit_NeoPixel.h>

// Circular Buffer
#include <CircularBuffer.hpp>

// NVS 
#include <Preferences.h>

// helix-axis
#include "GA.h"


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SERVICE_UUID        "dafffacb-1fb5-459e-8fcc-c5c9c3300004"
#define CHARACTERISTIC_UUID "deco000b-36e1-4688-b7f5-ea07361b26a8"
#define DEVICE_NAME         "BatterySaltLamp^0x0B^"

#define FADE_FRACTION 100
#define __BRIGHTNESS_SCALE__ 1.0
#define __BRIGHTNESS_KEY__ "brightness"

// Seeed Studio XIAO ESP32-C6
#define LED_PIN D10 // Seeed Studio XIAO ESP32-C6 external (4th pin down to right of USB controller)
#define LED_PIN_INDICATOR LED_BUILTIN // Seeed Studio XIAO ESP32-C6 internal LED
#define INDICATOR_RGB false // Seeed Studio XIAO ESP32-C6 internal LED is not RGB

// Waveshare ESP32-S3-Tiny
//#define LED_PIN 1 // Waveshare ESP32-S3-Tiny external (4th pin down the power and ground pin side)
//#define LED_PIN 38 // Waveshare ESP32-S3-Tiny internal RGBLED
//#define LED_PIN_INDICATOR 38 // Waveshare ESP32-S3-Tiny  internal RGBLED
//#define INDICATOR_RGB false // ESP32S3 Supermini internal RGBLED is RGB

// ESP32C3 SuperMini (Generic)
//#define LED_PIN 4 // ESP32C3 Supermini external (4th pin down to right of USB controller)
//#define LED_PIN_INDICATOR 8 // ESP32C3 Supermini internal RGBLED
//#define INDICATOR_RGB false // ESP32C3 Supermini internal RGBLED is not RGB

// ESP32S3 SuperMini (Generic)
//#define LED_PIN 13 // ESP32S3 Supermini external (4th pin down to right of USB controller)
//#define LED_PIN 48 // ESP32S3 Supermini internal RGBLED
//#define LED_PIN_INDICATOR 48 //PIN_NEOPIXEL // ESP32S3 Supermini internal RGBLED
//#define INDICATOR_RGB false // ESP32S3 Supermini internal RGBLED is RGB

// ESP32C6 SuperMini (Generic)
//#define LED_PIN 20 // ESP32C6 Supermini external (4th pin down to right of USB controller)
//#define LED_PIN 8 // ESP32C6 Supermini internal RGBLED
//#define LED_PIN_INDICATOR 8 //PIN_NEOPIXEL // ESP32SC6 Supermini internal RGBLED
//#define INDICATOR_RGB false // ESP32C6 Supermini internal RGBLED is RGB

// Adafruit QT Py ESP32-S3
// for Adafruit QT Py ESP32-S3 - NEOPIXEL_POWER == 38 and NEOPIXEL == 39
//#define LED_PIN 39 // Adafruit QT Py ESP32-S3 internal RGBLED
//#define LED_PIN_INDICATOR 39 // Adafruit QT Py ESP32-S3 internal RGBLED
//#define INDICATOR_RGB false // Adafruit QT Py ESP32-S3 internal RGBLED is RGB

#define LED_COUNT 50
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ400);
Adafruit_NeoPixel indicatorstrip(1, LED_PIN_INDICATOR, NEO_GRB + NEO_KHZ800);


CircularBuffer<int, 100> CommandsBuff;

Preferences preferences;

GA Engine;
int i = 0;
int fade_fraction = FADE_FRACTION;
float BrightnessScale = __BRIGHTNESS_SCALE__;

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

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        
        
        Serial.print(value);

        //int command = (int)value[0] - 48;
        
        int command = value.toInt();
        if (command == 0 && value != "0") {
            Serial.print("Can't convert to command.");
            Serial.println("*********");
            return;
        }
        
        if (!CommandsBuff.push(command))
        {
            DEBUG_ERROR("Overflowed circular buffer. Erasing head");
        }
        else
        {
            DEBUG_VERBOSE("Collected command %d", command);
        }
        //for (int i = 0; i < value.length(); i++)
        //  Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

void ReadBatteryVoltage()
{
    uint32_t Vbatt = 0;
    for(int i = 0; i < 16; i++) {
        Vbatt += analogReadMilliVolts(A0); // Read and accumulate ADC voltage
    }
    float Vbattf = 2 * Vbatt / 16 / 1000.0;     // Adjust for 1:2 divider and convert to volts
    Serial.print("bat: ");
    Serial.println(Vbattf, 3);                  // Output voltage to 3 decimal places
    delay(3);
}

void CheckCommands()
{
    if (CommandsBuff.isEmpty())
    {
        DEBUG_VERBOSE("No commands to process");
    }
    else
    {
        while (!CommandsBuff.isEmpty())
        {
          int command = CommandsBuff.pop();
          Commands cmd = static_cast<Commands>(command);
          //DEBUG_INFO("Received Command: %d, %s", command, cmd);

          ActionCommand(cmd);

        }
    }
}


void ActionIndicator(byte r, byte g, byte b, int pauselength=100)
{
    FlashIndicator(1);
}

void Brighten()
{
    if (BrightnessScale > 0.09) {
        DEBUG_INFO("PUSH WHOLE STEP, %f to", BrightnessScale);
        BrightnessScale = BrightnessScale + 0.1 >= 1 ? 1 : BrightnessScale + 0.1;
    } else {
        DEBUG_INFO("PUSH 1/10th STEP, %f to", BrightnessScale);
        BrightnessScale = BrightnessScale + 0.01 >= 1 ? 1 : BrightnessScale + 0.01;
    }
    DEBUG_INFO("%f", BrightnessScale);
    preferences.putFloat(__BRIGHTNESS_KEY__, BrightnessScale);
    strip.setBrightness(255 * BrightnessScale);
}

void Dim()
{
    if (BrightnessScale <= 0.1)
        BrightnessScale = BrightnessScale - 0.01 <= 0 ? 0 : BrightnessScale - 0.01;
    else
        BrightnessScale = BrightnessScale - 0.1 <= 0 ? 0 : BrightnessScale - 0.1;
    
    DEBUG_INFO("%f", BrightnessScale);
    preferences.putFloat(__BRIGHTNESS_KEY__, BrightnessScale);
    strip.setBrightness(255 * BrightnessScale);
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

void evolve_steps(int steps)
{
    DEBUG_INFO("Evolving %d steps", steps);
    for (int j = 0; j < steps; j++)
    {
        Engine.evolve();
    }
    i += steps;
}

void ActionCommand(Commands cmd)
{
    switch (cmd)
    {
        case BrightnessIncrease: // 0x00
            DEBUG_INFO("Increase Brightness");
            ActionIndicator(255, 255, 255);
            Brighten();
            ActionIndicator(0, 0, 0, 3);
            break;
        case BrightnessDecrease: // 0x01
            DEBUG_INFO("Decrease Brightness");
            ActionIndicator(128, 128, 128);
            Dim();
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

void FlashIndicator(int num_flashes)
{
    bool rgbled = true;
#if defined(INDICATOR_RGB)
    rgbled = false;
#endif    
    Serial.println("Flashing");
    for (int i = 0; i < num_flashes; i++)
    {
        if (rgbled) {
            // set color to red
            indicatorstrip.fill(0xFF0000);
            indicatorstrip.show();
        } else {
            digitalWrite(LED_PIN_INDICATOR, HIGH);
        }
        delay(200); // wait quarter second

        if (rgbled) {
            // turn off
            indicatorstrip.fill(0x000000);
            indicatorstrip.show();
        } else {
            digitalWrite(LED_PIN_INDICATOR, LOW);
        }
        delay(200); // wait quarter second
        
    }   
    delay(1000);
}

void setup() {

#if defined(INDICATOR_RGB)
    pinMode(LED_PIN_INDICATOR, OUTPUT);
#endif

    FlashIndicator(1);
    Serial.begin(115200);
    while ( !Serial && millis() < 10000 ) {
        delay(500) ;
    }

#if defined(NEOPIXEL_POWER)
    // If this board has a power control pin, we must set it to output and high
    // in order to enable the NeoPixels. We put this in an #if defined so it can
    // be reused for other boards without compilation errors
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, HIGH);
#endif
    // for Adafruit QT Py ESP32-S3 - NEOPIXEL_POWER == 38 and NEOPIXEL == 39

    FlashIndicator(2);


    Serial.println("Scanning...");

    preferences.begin("helix", false);
    BrightnessScale = preferences.getFloat("brightness", BrightnessScale);
    //BrightnessScale = 

    BLEDevice::init(DEVICE_NAME);
    BLEServer *pServer = BLEDevice::createServer();

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE
                                        );

    pCharacteristic->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Input Command String 0-17");
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    Debug.timestampOn();
    Debug.setDebugLevel(DBG_INFO);
    Engine.init();

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(255 * BrightnessScale);
    FlashIndicator(4);
}

void loop() {
    // put your main code here, to run repeatedly:
    // BLEScanResults foundDevices = *pBLEScan->start(scanTime, false);
    // Serial.print("Devices found: ");
    // Serial.println(foundDevices.getCount());
    // Serial.println("Scan done!");
    // pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory

    Serial.printf("Generation i = %d\n", i) ;
    // // evo
    // DEBUG_INFO("i = %d", i);
    i++;

    Engine.evolve();
    output_lights();

    //ReadBatteryVoltage(); // only if 200kohm resistor is hooked in (2:1 configuration (?) ) between battery in and A0.

    //delay(1000);
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