#include <BinaryOutput.h>

void setup()
{
  BINARYOUTPUT.begin();

  char stringBuf[255];
  char buff_1[124];
  BINARYOUTPUT.init_array_to_zero(buff_1, 124);

  Serial.println("START TESTS");
  BINARYOUTPUT.byte_to_binary_string(0x5, stringBuf);
  Serial.println("Test 1: Convert 0x5 to Binary: " + String(stringBuf));

  uint8_t test[5] = {
    0x05, 0x22, 0xFF, 0x00, 0x01  };
  Serial.println("Test 2: Convert {0x05, 0x22, 0xFF, 0x00, 0x01} to Binary: ");
  BINARYOUTPUT.byte_array_to_debug_binary_string(test, 5);

  Serial.println("Test 3:  Big 124char Zero-filled array:");
  BINARYOUTPUT.byte_array_to_debug_binary_string(buff_1, 124);
  
  Serial.println("Test 4 -- Big 124char array, 2nd Byte: 0x5, 123rd Byte: 0xFE:");
  buff_1[1] = 0x05;
  buff_1[122] = 0xFE;  
  BINARYOUTPUT.byte_array_to_debug_binary_string(buff_1, 124);

  Serial.println("END TESTS");
}

void loop()
{
}

