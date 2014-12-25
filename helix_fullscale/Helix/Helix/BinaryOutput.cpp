/*
  BinaryOutput.h - Outputs Binary String Representations of Bytes and
  Strings.

  Copyright (c) 2013 Rosangela Canino-Koning.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  Binary String Outputs

  Converts Bytes and Arrays into their string binary representations for
  debugging purposes.
*/

#include "BinaryOutput.h"

BinaryOutput_Class::BinaryOutput_Class() {};

void BinaryOutput_Class::begin()
{
    Serial.begin(9600);
}

void BinaryOutput_Class::init_array_to_zero(char* buf, int size)
{
    init_array_to_zero((uint8_t*) buf, size);
}

void BinaryOutput_Class::init_array_to_zero(uint8_t* buf, int size)
{
    for (int i = 0; i < size; i++)
    {
        buf[i] = 0;
    }
}

// assumes an out buffer of at least 9 bytes
void BinaryOutput_Class::byte_to_binary_string(uint8_t aByte, char *out)
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
void BinaryOutput_Class::int_to_binary_string(uint16_t aInt, char *out)
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

void BinaryOutput_Class::byte_array_to_debug_binary_string(char *buf, int size)
{
    byte_array_to_debug_binary_string( (uint8_t*) buf, size);
}

void BinaryOutput_Class::byte_array_to_debug_binary_string(uint8_t *buf, int size)
{
    char lByte[9];

    String lContent;

    //Serial.println("Size: " + String(size));

    int i;
    for (i = 0; i < size; i++)
    {
        if ((i % 4) == 0)
            lContent = String(i) + ":   ";

        byte_to_binary_string(buf[i], lByte);

        lContent += String(lByte) + " ";

        if ((i % 4) == 3)
        {
            Serial.println(lContent);
        }   
    }
    if ((i % 4) != 0)
        Serial.println(lContent);
}

void BinaryOutput_Class::test()
{
  char stringBuf[255];
  char buff_1[124];
  init_array_to_zero(buff_1, 124);

  Serial.println("START TESTS");
  byte_to_binary_string(0x5, stringBuf);
  Serial.println("TEST 1: Convert 0x5 to Binary: " + String(stringBuf));

  uint8_t test[5] = {
    0x05, 0x22, 0xFF, 0x00, 0x01  };
  Serial.println("TEST 2: Convert {0x05, 0x22, 0xFF, 0x00, 0x01} to Binary: ");
  byte_array_to_debug_binary_string(test, 5);

  Serial.println("Test 3 -- Big 124char Zero-filled array:");
  byte_array_to_debug_binary_string(buff_1, 124);
  
  Serial.println("Test 4 -- Big 124char array, 2nd Byte: 0x5, 123rd Byte: 0xFE:");
  buff_1[1] = 0x05;
  buff_1[122] = 0xFE;  
  byte_array_to_debug_binary_string(buff_1, 124);

  Serial.println("END TESTS");
}



BinaryOutput_Class BINARYOUTPUT = BinaryOutput_Class();

