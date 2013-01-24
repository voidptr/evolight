///
/// @mainpage	Helix
/// @details	<#details#>
/// @n
/// @n
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Rosangela Canino Koning
/// @author	Sube Studio
/// @date	1/22/13 3:30 PM
/// @version	<#version#>
///
/// @copyright	© Rosangela Canino Koning, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///


///
/// @file	Helix.pde
/// @brief	Main sketch
/// @details	<#details#>
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Rosangela Canino Koning
/// @author	Sube Studio
/// @date	1/22/13 3:30 PM
/// @version	<#version#>
///
/// @copyright	© Rosangela Canino Koning, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
/// @n
///


// Core library for code-sense
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#elif defined(ARDUINO) && (ARDUINO < 100) // Arduino 23 specific
#include "WProgram.h"
#else // error
#error Platform not defined
#endif

// Include application, user and local libraries
#include "LocalLibrary.h"
#include "Wire.h"
#include "FastSPI_LED_Chipkit_Minimal.h"
#include "BinaryOutput.h"
#include "CypressCapsense.h"

// Define variables and constants
///
/// @brief	Name of the LED
/// @details	Each board has a LED but connected to a different pin
///
uint8_t myLED = 13;

#define NUM_LEDS 30
#define LED 7
#define FETPIN 6

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

#define PIN 4

#define CapsenseInitAddress 0x00
#define CapsenseNewAddress 0x20


#define Port0 0b0000000000000100
#define Port1 0b0001000000000000
#define Port2 0b0000010000000000
#define Port3 0b0000100000000000
#define Port4 0b0000001000000000
#define Port5 0b0000000100000000
#define Port6 0b0000000000000010
#define Port7 0b0000000000010000
#define Port8 0b0000000000000001
#define Port9 0b0000000000001000

uint16_t Ports[10] = {Port0, Port1, Port2, Port3, Port4, Port5, Port6, Port7, Port8, Port9};

void get_capsense_firmware()
{
    char buf[1];
    buf[0] = 0xFF;
    CypressCapsenseC8YC20.fetchFirmwareRevision(CapsenseNewAddress, buf);
    BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1);
}

void reset_capsense()
{
    digitalWrite(FETPIN, LOW);
    delay(10);
    digitalWrite(FETPIN, HIGH);
    delay(100);
}

void capsense_setup()
{
    BINARYOUTPUT.begin();
    pinMode(FETPIN, OUTPUT); // fet to control power to the cypress bus
    
    reset_capsense();
    
    // change the address from the default.
    // CypressCapsenseC8YC20.changeDeviceAddress(CapsenseInitAddress, CapsenseNewAddress);
    
    // fetch the firmware revision
    get_capsense_firmware();    
    
    // set up the device to read capsense on all ports.
    CypressCapsenseC8YC20.setupDevice(CapsenseNewAddress, 0x0000,
                                      (Port0 | Port1 | Port2 | Port3 | Port4 | Port5 | Port6 | Port7 | Port8 | Port9));
}

void led_setup()
{
    pinMode(myLED, OUTPUT); // built in led
}

void fastspi_setup()
{    
    FastSPI_LED.setLeds(NUM_LEDS);
    FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
    FastSPI_LED.setPin(PIN);
    FastSPI_LED.init();
    FastSPI_LED.start();
    
    leds = (struct CRGB*)FastSPI_LED.getRGBData();
}

///
/// @brief	Setup
/// @details	Define the pin the LED is connected to
///
// Add setup code
void setup() {    
    // setup Capsense Stuff
    capsense_setup();
    
    // setup LED stuff
    led_setup();
    
    // fastspi setup
    fastspi_setup();
}

int loopct = 0;


///
/// @brief	Loop
/// @details	Call blink
///
// Add loop code
void loop() {
    blink(myLED, 3, 333); 
    
    if (loopct++ > 20)
    {
        loopct = 0;
        reset_capsense();
        Serial.println("RESETTING CAPSENSE");
    }
    
    char stringBuf[20];
    int touchstatus = CypressCapsenseC8YC20.fetchTouchStatus(CapsenseNewAddress);
    BINARYOUTPUT.int_to_binary_string(touchstatus, stringBuf);
    Serial.print("Touch Status: ");
    Serial.println(stringBuf);
    
    Serial.print("Raw Counts: ");
    for (int i = 0; i < 10; i++)
    {
        int ct = CypressCapsenseC8YC20.fetchRawCounts(CapsenseNewAddress, Ports[i]);
        Serial.println("Port: " + String(i) + "(" + String( Ports[i]) + ") -------------- " + String(ct));
    }
    
    // Fade in/fade out
    for(int j = 0; j < 3; j++ ) {
        memset(leds, 0, NUM_LEDS * 3);
        for(int k = 0; k < 256; k++) {
            for(int i = 0; i < NUM_LEDS; i++ ) {
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
            for(int i = 0; i < NUM_LEDS; i++ ) {
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
    
    
    delay(100);
}
