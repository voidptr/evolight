#include <FastSPI_LED.h>
#include "utils.h"

#define NUM_LEDS 180

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

#define PIN 4

int led = 13;

void blink() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}

void print_free_memory() {
  Serial.print("Free Ram: ");
  Serial.println( freeRam() );
}

void setup()
{
  // init the serial
  Serial.begin(9600);
  
  pinMode(led, OUTPUT);
  
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2811);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_SM16716);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD6803);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_HL1606);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_595);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);
  

  FastSPI_LED.setPin(PIN);
  
  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
}

void loop() { 
  Serial.println("BLINKING");
  blink();
  Serial.println("BLINKING DONE");
  
  
  print_free_memory();
  Serial.println("ONE AT A TIME");
  // one at a time
  for(int j = 0; j < 3; j++) { 
    
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      memset(leds, 0, NUM_LEDS * 3);
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      delay(10);
    }
  }
  Serial.println("DONE ONE AT A TIME");

  print_free_memory();

  Serial.println("Growing Receding");
  // growing/receeding bars
  for(int j = 0; j < 3; j++) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      delay(10);
    }
    for(int i = NUM_LEDS-1 ; i >= 0; i-- ) {
      switch(j) { 
        case 0: leds[i].r = 0; break;
        case 1: leds[i].g = 0; break;
        case 2: leds[i].b = 0; break;
      }
      FastSPI_LED.show();
      delay(1);
    }
  }
  Serial.println("DONE Growing Receding");
  print_free_memory();
  
  
  // Fade in/fade out
  Serial.println("Fade In/Out");
  for(int j = 0; j < 3; j++ ) { 
    //print_free_memory();
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
      //print_free_memory();
    }
    print_free_memory();
    for(int k = 255; k >= 0; k--) { 
      print_free_memory();
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      delay(3);
      print_free_memory();
    }
  }
  Serial.println("Done Fade In/Out");
  print_free_memory();
}

