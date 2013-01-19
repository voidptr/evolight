#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "utils.h"

int led = 13;

void debug_setup() 
{
  pinMode(PIN_LED1, OUTPUT);
    
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
  //print_free_memory();

  // blink led
  digitalWrite(PIN_LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(PIN_LED1, LOW);    // turn the LED off by making the voltage LOW
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

void debug(String string)
{
    Serial.println(string);
}
