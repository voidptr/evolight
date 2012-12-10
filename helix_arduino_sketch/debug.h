#include "utils.h"

int led = 13;

void debug_setup() 
{
  pinMode(led, OUTPUT);
    
  // init the serial
  Serial.begin(9600);
}

void print_free_memory() 
{
  Serial.print("Free Ram: ");
  Serial.println( freeRam() );
}

void debug()
{
  print_free_memory();

  // blink led
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(100);      
}

void debug(int count)
{
    Serial.println(count);
}

void debug(char * string)
{
    Serial.println(string);
}

