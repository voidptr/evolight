#include "SPI.h"
#include "WS2801.h"
#include "CapacitiveSensor.h"
#include "evolight.h"
#include "colors.h"
#include "cap_buttons.h"
#include "light_map.h"
//#include "utils.h"

/*****************************************************************************
Evolight
*****************************************************************************/

// TODO
// import and use CapSense library

////// Defaults
// hardware setup
int dataPin = 2; // green
int clockPin = 3; // blue
int led = 13;

bool __using_delays__ = false;

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(60, dataPin, clockPin);

void conditional_delay(int wait) {
  if (__using_delays__) {
    delay(wait);
  }
}

void print_free_memory() {
  Serial.print("Free Ram: ");
  Serial.println( freeRam() );
}

void display_population(uint8_t wait) {
  uint32_t c;
  
  //Serial.println(strip.numPixels());
  
  int length = strip.numPixels();
  if (length > __length__) {
    length = __length__;
  }
  
  //The hardware is set up with sixty lights, the first six of which are status and utility lights.
  //The rest of the lights are organized zig-zag, so we have to account for that.
  
  for(int org = 0, row = 0; org < __population_size__; org += 4, row++) // display the ones that go from left to right
  {   
    for(int light = 0; light < __length__; light++)
    {
      //c = ToRGB(population[org][light*3], population[org][(light*3) + 1], population[org][(light*3) + 2]);
      c = Color(population[org][light][0], population[org][light][1], population[org][light][2]);
      strip.setPixelColor(organism_lightmap[((row*__length__)+light)], c);
    }
  }    
  
  strip.show();
  conditional_delay(wait);
}

void evolve() {  
  
  select_and_mutate();  
  calculate_locus_differences__population();
  
  conditional_delay(500);
}

void check_locus_button() {
  
  for (int locus = 0; locus < __locus_pad_count__; locus++)
  {
    if (LocusPads[locus]) {
      flash_locus_lights(locus);
      randomize_locus(locus);
    }
  }
}

void check_reset() {
  if (ResetPad > 5) {
    reset_ga();
    signal_reset_light();
    cycle_status_light();
    Serial.println("Resetting Simulation");
  } 
} 

void flash_reset_light() {
  strip.setPixelColor(reset_light, Color(0, 0, 0));
  strip.show();
  conditional_delay(100);
  strip.setPixelColor(reset_light, Color(64, 64, 64));
  strip.show();
}

void flash_locus_lights(int locus) {
  for (int org = 0; org < __org_pad_count__; org++) {
    strip.setPixelColor(locus_lightmap[locus][org], Color(0, 0, 0));
  }
  strip.show();
  delay(200);
  for (int org = 0; org < __org_pad_count__; org++) {
    strip.setPixelColor(reset_light, Color(255, 255, 255));
  }
  strip.show();
}

void signal_reset_light() {
  strip.setPixelColor(reset_light, Color(0, 0, 0));
  strip.show();
  delay(200);
  strip.setPixelColor(reset_light, Color(255, 255, 255));
  strip.show();
}

void cycle_status_light() {
  strip.setPixelColor(mut_light, Color(0, 0, 0));
  strip.show();
  delay(200);
  if (__per_site_mutation_rate__ == 0.01) { strip.setPixelColor(mut_light, Color(0, 255, 0)); }
  else if (__per_site_mutation_rate__ == 0.02) { strip.setPixelColor(mut_light, Color(84, 170, 0)); }
  else if (__per_site_mutation_rate__ == 0.04) { strip.setPixelColor(mut_light, Color(128, 128, 0)); }
  else if (__per_site_mutation_rate__ == 0.08) { strip.setPixelColor(mut_light, Color(170, 84, 0)); }
  else if (__per_site_mutation_rate__ == 0.16) { strip.setPixelColor(mut_light, Color(212, 42, 0)); }
  else if (__per_site_mutation_rate__ == 0.32) { strip.setPixelColor(mut_light, Color(255, 0, 0)); }
  strip.show();
} 

void check_mutation_rate() {

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (MutationRatePad) {
    cycle_mutation_rate();
    cycle_status_light();
  }
} 

void setup() {
    
//  cs_23_33.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  
  pinMode(led, OUTPUT);
    
  // init the serial
  Serial.begin(9600);
  
  
  // random number seed
  randomSeed(analogRead(0));
  
  init_random_population();

  strip.begin();
 
  // Update LED contents, to start they are all 'off'
  strip.show();
  // strip.setPixelColor(0, Color(0, 0, 255));
  
  cycle_status_light();
}

void loop() { 
  
  update_button_state();
  
  check_reset();
  check_mutation_rate();
  check_locus_button();
  
  evolve();
 
  display_population(2);
  flash_reset_light();
  
  print_free_memory();
  
//  print_highestfitnesses();
}



