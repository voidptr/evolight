// Core library for code-sense
#include "IDEIncludes.h"

#ifndef DEBUG_H
#define DEBUG_H

#include "ConfigurationDefines.h"

#ifndef __SERIAL_SPEED__
#define __SERIAL_SPEED__ 9600
#endif

class Debug {
    
private:
    Debug()
    {
        Serial.begin(__SERIAL_SPEED__);
        //buf = new char [64];
        
        for (int i = 0; i < 64; i++)
        {
            buf[i] = 0;
        }
        
        pinMode(PIN_LED1, OUTPUT);
    }
    Debug(Debug const& copy);
    Debug& operator=(Debug const& copy);
    
    char buf[64];

public:
    static Debug& getInstance()
    {
        static Debug instance;
        return instance;
    }
    
    static void debug()
    {
        getInstance();
        
        digitalWrite(PIN_LED1, HIGH);   // set the LED on
        delay(200);              // wait for a second
        digitalWrite(PIN_LED1, LOW);    // set the LED off
        delay(200);              // wait for a second
        
        digitalWrite(PIN_LED1, HIGH);   // set the LED on
        delay(50);              // wait for a second
        digitalWrite(PIN_LED1, LOW);    // set the LED off
        delay(50);              // wait for a second

        digitalWrite(PIN_LED1, HIGH);   // set the LED on
        delay(50);              // wait for a second
        digitalWrite(PIN_LED1, LOW);    // set the LED off
        delay(50);              // wait for a second
        
        digitalWrite(PIN_LED1, HIGH);   // set the LED on
        delay(200);              // wait for a second
        digitalWrite(PIN_LED1, LOW);    // set the LED off
        delay(200);              // wait for a second
    }
    
    static void debug(int count)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(count);
    }
    
    static void debug(char * string)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(string);
    }

    static void debug(const String& string)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(string);
    }
    
    static void debugln()
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.println();
    }
    
    static void debugln(int count)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.println(count);
    }
    
    static void debugln(char * string)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.println(string);
    }
    
    static void debugln(const String& string)
    {
        getInstance();
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.println(string);
    }
    
    static void debug_binary(uint32_t val)
    {
        getInstance();
        int32_to_binary_string(val, getInstance().buf);
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(getInstance().buf);
    }
    
    static void debug_binary(uint16_t val)
    {
        getInstance();
        int_to_binary_string(val, getInstance().buf);
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(getInstance().buf);
        
    }
    
    static void debug_binary(uint8_t val)
    {
        getInstance();
        byte_to_binary_string(val, getInstance().buf);
        if (__OUTPUT_DEBUG_LEVEL__)
            Serial.print(getInstance().buf);
    }
private:
    // assumes an out buffer of at least 9 bytes
    static void byte_to_binary_string(uint8_t aByte, char *out)
    {
        uint8_t lMask = 0x1;
        for (int i = 7; i >= 0; i--)
        {
            if ((aByte & lMask) == 1)
                out[i] = '1';
            else
                out[i] = '0';
            //            out[i] = (aByte & lMask);
            aByte >>= 1;
        }
        out[8] = '\0'; // null char terminator
        //Serial.println(out);
    }
    
    // assumes an out buffer of at least 17 bytes
    static void int_to_binary_string(uint16_t aInt, char *out)
    {
        uint16_t lMask = 0x0001;
        for (int i = 15; i >= 0; i--)
        {
            if ((aInt & lMask) == 1)
                out[i] = '1';
            else
                out[i] = '0';
            //            out[i] = (aByte & lMask);
            aInt >>= 1;
        }
        out[16] = '\0'; // null char terminator
        //Serial.println(out);
    }
    
    // assumes an out buffer of at least 33 bytes
    static void int32_to_binary_string(uint32_t aInt, char *out)
    {
        uint32_t lMask = 0x00000001;
        for (int i = 31; i >= 0; i--)
        {
            if ((aInt & lMask) == 1)
                out[i] = '1';
            else
                out[i] = '0';
            //            out[i] = (aByte & lMask);
            aInt >>= 1;
        }
        out[32] = '\0'; // null char terminator
        //Serial.println(out);
    } 
};


#endif