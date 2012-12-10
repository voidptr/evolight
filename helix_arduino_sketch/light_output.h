#include "light_map.h"
#include "light_hardware.h"
#include "colors.h"

//const int __num_organisms__ = 10;
//const int __max_length__ = 30;

void setup_light_output() {
  setup_led_strips();
}

/*
void display_population(uint8_t wait=500) {
  uint32_t c;
  
  for(int org = 0, row = 0; org < __population_size__; org += 4, row++) // display the ones that go from left to right
  {   
    for(int light = 0; light < __length__; light++)
    {
      //c = ToRGB(population[org][light*3], population[org][(light*3) + 1], population[org][(light*3) + 2]);
      c = Color(population[org][light][0], population[org][light][1], population[org][light][2]);
      set_light(organism_lightmap[((row*__length__)+light)], population[org][light][0], population[org][light][1], population[org][light][2]);
      //strip.setPixelColor(organism_lightmap[((row*__length__)+light)], c);
    }
  }    
  
  show_lights();
  
  conditional_delay(wait);
} */

/*
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

void cycle_status_light(int state) {
  strip.setPixelColor(mut_light, Color(0, 0, 0));
  strip.show();
  delay(200);
  if (state == 1) { strip.setPixelColor(mut_light, Color(0, 255, 0)); }
  else if (state == 2) { strip.setPixelColor(mut_light, Color(84, 170, 0)); }
  else if (state == 3) { strip.setPixelColor(mut_light, Color(128, 128, 0)); }
  else if (state == 4) { strip.setPixelColor(mut_light, Color(170, 84, 0)); }
  else if (state == 5) { strip.setPixelColor(mut_light, Color(212, 42, 0)); }
  else if (state == 6) { strip.setPixelColor(mut_light, Color(255, 0, 0)); }
  strip.show();
} */

