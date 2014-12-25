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
// Core library for code-sense
#include "IDEIncludes.h"

#ifndef BINARY_OUTPUT_H
#define BINARY_OUTPUT_H

#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


class BinaryOutput_Class
{

public:
    BinaryOutput_Class();
    void begin();
    void init_array_to_zero(char* buf, int size);
    void init_array_to_zero(uint8_t* buf, int size);
    void byte_to_binary_string(uint8_t aByte, char *out);
    void int_to_binary_string(uint16_t aInt, char *out);
    void byte_array_to_debug_binary_string(char *buf, int size);
    void byte_array_to_debug_binary_string(uint8_t *buf, int size);
    void test();
};

extern BinaryOutput_Class BINARYOUTPUT;

#endif
