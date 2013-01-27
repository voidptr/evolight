#include <Wire.h>
#include <CypressCapsense.h>
#include <BinaryOutput.h>

#define CapsenseInitAddress 0x00

#define Port0 0b0000000000000100
#define Port1 0b0001000000000000
#define Port2 0b0000010000000000
#define Port3 0b0000100000000000
#define Port4 0b0000001000000000
#define Port5 0b0000000100000000
#define Port6 0b0000000000000010
#define Port7 0b0000000000010000
#define Port8 0b0000000000000001
#define Port9 0b0000000000001000

uint16_t Ports[10] = {Port0, Port1, Port2, Port3, Port4, Port5, Port6, Port7, Port8, Port9};

void setup()
{
  BINARYOUTPUT.begin();

  char buf[1];
  buf[0] = 0xFF;

  // fetch the firmware revision. This will indicate if the device is listening at the default address  
  CypressCapsenseC8YC20.fetchFirmwareRevision(CapsenseInitAddress, buf);  
  BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1); 
 
  // set up the device to read capsense on all ports.
  CypressCapsenseC8YC20.setupDevice(CapsenseInitAddress, 0x0000,      
  (Port0 | Port1 | Port2 | Port3 | Port4 | Port5 | Port6 | Port7 | Port8 | Port9));
}

void loop()
{
    char stringBuf[20];
    int touchstatus = CypressCapsenseC8YC20.fetchTouchStatus(CapsenseInitAddress);
    BINARYOUTPUT.int_to_binary_string(touchstatus, stringBuf);
    Serial.print("Touch Status: ");
    Serial.println(stringBuf);

    Serial.println("Raw Counts: ");
    for (int i = 0; i < 10; i++)
    {
        int ct = CypressCapsenseC8YC20.fetchRawCounts(CapsenseInitAddress, Ports[i]);
        Serial.println("Port: " + String(i) + "(" + String( Ports[i]) + ") -------------- " + String(ct));
    }
    
    delay(1000);
}

