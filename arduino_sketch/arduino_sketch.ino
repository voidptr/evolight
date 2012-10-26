#include "SPI.h"
#include "WS2801.h"
#include "evolight.h"
#include "colors.h"
//#include "utils.h"

/*****************************************************************************
Evolight
*****************************************************************************/

////// Defaults
// hardware setup
int dataPin = 2;
int clockPin = 3;
int led = 13;
int resetGAPin = 6;
int mutationRatePin = 5;

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(25, dataPin, clockPin);

// the loop routine runs over and over again forever:
void blinkled() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
}

void display_fittest(uint8_t wait) {
  uint32_t c;
  
  int length = strip.numPixels();
  if (length > __length__) {
    length = __length__;
  }
  
  for (int i=0; i < length; i++) {   
      c = Color(population[0][i*3], population[0][(i*3) + 1], population[0][(i*3) + 2]);
    
      strip.setPixelColor(i, c);
//      strip.show();
  }
  strip.show();
  delay(wait);
}

void evolve() {  
  
  select_and_mutate();  
  calculate_fitnesses();
  print_highestfitness();
}

void check_reset() {
  // read the state of the pushbutton value:
  int buttonState = digitalRead(resetGAPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    reset_ga();
  }
}

void check_mutation_rate() {
  // read the state of the pushbutton value:
  int buttonState = digitalRead(mutationRatePin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    cycle_mutation_rate();
  }
}

void setup() {
  
  pinMode(led, OUTPUT);
    
  // init the serial
  Serial.begin(9600);
  
  
  // random number seed
  randomSeed(analogRead(0));
  
  init_random_population();

  strip.begin();
 
  // Update LED contents, to start they are all 'off'
  strip.show();
}

void loop() { 
  check_reset();
  //check_mutation_rate();
  
  evolve();
 
  display_fittest(2);

}



