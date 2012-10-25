#include "SPI.h"
#include "WS2801.h"

/*****************************************************************************
Evolight
*****************************************************************************/

////// Defaults
// hardware setup
unsigned short dataPin = 2;
unsigned short clockPin = 3;
unsigned short led = 13;
// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
WS2801 strip = WS2801(25, dataPin, clockPin);

/// evolution program setup
const char __length__ = 6;
const char __traits__ = 3;
const char __population_size__ = 10;
const float __per_site_mutation_rate__ = 0.01;
const unsigned short __max_trait_val__ = 256;
const unsigned short __genome_length__ = __length__ * __traits__;
const float __selection_threshold__ = 0.25;

char population[__population_size__][__genome_length__]; // population
char candidates[__population_size__][__genome_length__]; // candidates for repro
unsigned short fitnesses[__population_size__]; // index and fitness

char get_rand_trait() {
  return char(random(0, 256));
}

unsigned short is_mut() {
//  Serial.println("is_mut()");
  return random(0, 100) < __per_site_mutation_rate__ * 100;
}

void init_random_population() {
  Serial.println("init_random_population()");
  
  for (unsigned short i=0; i < __population_size__; i++) {  
    for (unsigned short j=0; j < __genome_length__; j++) {     
      population[i][j] = get_rand_trait();
    } 
  }
 
  calculate_fitnesses();
  
  Serial.println("donezomg");
}

// mutate a candidate genome. this is analogous to errors during gametogenensis. 
void mutate(char genome[]) { 
//  Serial.println("mutate()");
  for (unsigned short i = 0; i < __genome_length__; i++) {
    if (is_mut()) { // mutate
      genome[i] = get_rand_trait();
    }
  }
}

unsigned short calculate_fitness( char genome[] )
{
//  Serial.println("calculate_fitness()");

  unsigned short colordiff = 0, rdiff = 0, gdiff = 0, bdiff = 0, diffs = 0;
  for (unsigned short i = 3; i < __genome_length__ - 2; i += 3) {
//    Serial.println(colordiff);
    rdiff = abs( genome[i-3] - genome[i] );
//    Serial.println(rdiff);
    gdiff = abs( genome[i-2] - genome[i+1] );
//    Serial.println(gdiff);
    bdiff = abs( genome[i-1] - genome[i+2] );
//    Serial.println(bdiff);
    
    //diffs = (rdiff * rdiff) + (gdiff * gdiff) + (bdiff * bdiff);
    diffs = rdiff + gdiff + bdiff;
    Serial.print(diffs);
    Serial.print(" ");
    
    colordiff += (rdiff + gdiff + bdiff);
    
    
  }
  Serial.print(" - ");
  Serial.println(colordiff);
//  Serial.println();
  
  return colordiff;
}

// this overwrites whatever's in the target slot with whatever the source was
void copy_genome(char source[], char target[]) {
//  Serial.println("copy_genome()");
  for (unsigned short i = 0; i < __genome_length__; i++) {
    target[i] = source[i];
  } 
}

// selects from among the population (assumed to be sorted)
// to population the candidate breeders arrays
// mutate the candidates prior to recombination
void select_candidates__hybrid() {
  Serial.println("select_candidates__hybrid()");
  unsigned short random_index = 0;
  // 90% of candidates are from the __selection_threshold__% (25%)
  for (unsigned short i = 0; i < (__population_size__ * 0.8); i++) {
    random_index = random(0, (__population_size__ * __selection_threshold__));
    copy_genome(population[random_index], candidates[i]);
    mutate(candidates[i]);
  }
  // 10% of candidates are from the __selection_threshold__% (25%)
  for (unsigned short i = 0; i < (__population_size__ * 0.2); i++) {
    random_index = random((__population_size__ * __selection_threshold__), 
      (__population_size__ * (__selection_threshold__ + 0.5)));
    copy_genome(population[random_index], candidates[i]);
    mutate(candidates[i]);
  }
}

// perform the complete recombination, filling up the population
void recombine_all_candidates() {
  Serial.println("recombine_all_candidates()");
  for (unsigned short i = 0; i < (__population_size__ - 1); i+= 2) {
    unsigned short candidateX = random(0, __population_size__);
    unsigned short candidateY = random(0, __population_size__);
    
    unsigned short posA = random(0, __length__);
    unsigned short posB = random(0, __length__);
    
    bool recomb = false;
    for (unsigned short j = 0; j < __genome_length__; j++) {
      if (j == posA || j == posB) {
        recomb = !recomb;
      }
      
      if (recomb) {
        population[i][j] = candidates[candidateX][j];
        population[(i+1)][j] = candidates[candidateY][j];
      } else {
        population[i][j] = candidates[candidateY][j];
        population[i+1][j] = candidates[candidateX][j];
      }
    }    
  }
}

void calculate_fitnesses() {
  Serial.println("calculate_fitnesses()");
  for (unsigned short i =0; i < __population_size__; i++) {
    fitnesses[i] = calculate_fitness( population[i] );
  }
  
  //quicksort_genomes(fitnesses, 0, __population_size__-1);
}

void quicksort_genomes(unsigned short a[], unsigned short l, unsigned short r){            //a=Array, l=left border, r=right border
  Serial.print("Sorting: ");
  Serial.print(l);
  Serial.print(" ");
  Serial.print(r);
  Serial.println();
  
//  Serial.println("quicksort_genomes()");
  if ( r>l ){                              //as long that more then 1 followingelement exists do
    unsigned short i = l-1, j = r, tmp;                  //Initialising the variables with the borders
    for(;;){                        //endless loop; stops if i>=j
      while(a[++i]<a[r]);                  //increm., until found bigger element
      while(a[--j]>a[r] && j>i);            //decrem., until found smaler element
      if(i>=j) break;                   //stops run if pounsigned shorter meet
      tmp=a[i];                        //exchange smaller with bigger element
      a[i]=a[j];                       //exchange smaller with bigger element
      a[j]=tmp;                        //exchange smaller with bigger element
      // RCK: and now we add the swapping of the back-end genomes that go along.
      swap_genomes(i, j);
    }
    tmp=a[i];           //exchange dividing element
    a[i]=a[r];          //exchange dividing element
    a[r]=tmp;            //exchange dividing element
    // RCK: and now swap the pivot genomes
    swap_genomes(i, r);
    
    quicksort_genomes(a, l, i-1);                  //recursive call of function for left
    quicksort_genomes(a, i+1, r);                  //recursive call of function for right
  }
  Serial.println("DONE");
}

void swap_genomes(unsigned short index1, unsigned short index2) {
  //Serial.println("swap_genomes()");
  char tmp = 0;
  for (unsigned short i=0; i < __genome_length__; i++) {
    tmp = population[index1][i];
    population[index1][i] = population[index2][i];
    population[index2][i] = tmp;
  }
}


void evolve() {  
  Serial.println("evolve()");
  select_candidates__hybrid();  
  recombine_all_candidates();
  calculate_fitnesses();
  
  Serial.println(fitnesses[0]);
}

// the loop routine runs over and over again forever:
void blinkled() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}

void setup() {
  
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  
  // init the serial
  Serial.begin(9600);
  
  // send an unsigned shortro:
  Serial.println("OHAI Serial is working");
  Serial.println(); 
  
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(500);               // wait for a second
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW

  
  // random number seed
  randomSeed(analogRead(0));
  
  // init the population
  init_random_population();
  
  

 
  
  // init the hardware
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
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
      strip.show();
  }
  delay(wait);
}

void loop() {
  // Some example procedures showing how to display to the pixels
  Serial.println("Starting Loop");
  
  blinkled();
  
  evolve();
  
  display_fittest(2000);
  
  //colorWipe(Color(255, 0, 0), 50);
  colorWipe(Color(0, 0, 0), 50);
  //colorWipe(Color(0, 255, 0), 50);
  //colorWipe(Color(0, 0, 0), 50);
  //colorWipe(Color(0, 0, 255), 50);
  //colorWipe(Color(0, 0, 0), 50);
  //rainbow(20);
  //rainbowCycle(20);
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
