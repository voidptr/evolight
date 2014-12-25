#include <FastSPI_LED.h>

#include <Wire.h>
#include <CypressCapsense.h>
#include <BinaryOutput.h>


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

byte TestAddress = 0x00;
bool FoundDevice = false;

//########### THIS HERE IS THE KEY BIT FOR REPROGRAMMING ##########
byte TargetReprogramAddress = 0x60;
bool ChangeAddress = false;
//#################################################################

//########### Testing LEDs from pin 4 ############
#define NUM_LEDS 20
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;
#define PIN 5
//################################################

void setup()
{
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  FastSPI_LED.setPin(PIN);
  FastSPI_LED.init();
  FastSPI_LED.start();
  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 
  
  
  BINARYOUTPUT.begin();

  byte foundaddr = setup_scan();
  if (foundaddr != 255)
  {
    Serial.println();
    Serial.print("FOUND IT AT ADDR ");
    Serial.println((int)foundaddr);
    BINARYOUTPUT.byte_array_to_debug_binary_string(&foundaddr, 1);  
    TestAddress = foundaddr; // assign it 
    FoundDevice = true;
  }
  else
  {
    Serial.println("COULD NOT FIND DEVICE");
  }
  
  
  
}

byte setup_scan()
{
  byte addr = 0x00;
  byte found = 0x00;
  for (int i = 0; i < 100; i++)
  {
    if (test_for_device(addr))
    {
      Serial.print("FOUND (int): ");
      Serial.println(i);
      found = addr;      
    }    
    addr++;
  }
  return found;
}

bool test_for_device(byte addr)
{
    char buf[1];
    buf[0] = 0xFF;
    
    Serial.print("Testing Address: ");
    Serial.println((int)addr);

    // fetch the firmware revision. This will indicate if the device is listening at the default address  
    CypressCapsenseC8YC20.fetchFirmwareRevision(addr, buf);  
    byte response = buf[0];    
         
    if (response != 0xFF)
    { 
      Serial.println("FOUND A DEVICE. Version: ");
      BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1);
      // set up the device to read capsense on all ports.           
      
      byte configtarget = addr;
      
      if (ChangeAddress && addr != TargetReprogramAddress)
      { 
        Serial.print("CHANGING ADDRESS FROM ");
        Serial.print((int)addr);
        Serial.print(" TO ");
        Serial.println((int)TargetReprogramAddress);
        
        // change the address
        CypressCapsenseC8YC20.changeDeviceAddress(addr, TargetReprogramAddress);  
//        CypressCapsenseC8YC20.changeDeviceAddress(0, 54);  
        
        Serial.print("CHANGING ADDRESS FROM ");
        Serial.print((int)addr);
        Serial.print(" TO ");
        Serial.println((int)TargetReprogramAddress);
        
        // fetch the firmware revision from the new address
        buf[0] = 0xFF;  
        
        CypressCapsenseC8YC20.fetchFirmwareRevision(TargetReprogramAddress, buf);  
        
        byte resp = buf[0];
        BINARYOUTPUT.byte_array_to_debug_binary_string(buf, 1);
        if (resp != 0xFF)
        {
          Serial.println("SUCCESS!");
          configtarget = TargetReprogramAddress;
        }
        else
        {
          Serial.println("FAILURE! (pausing)");
          delay(2000);
        }              
      }
      else if( addr == TargetReprogramAddress )
      {
        Serial.println("Found it, but already at reprogram target address.");
      }
      
      Serial.print("CONFIGURING DEVICE @ ");
      Serial.println((int)configtarget);
      CypressCapsenseC8YC20.setupDevice(configtarget, 0x0000,      
      (Port0 | Port1 | Port2 | Port3 | Port4 | Port5 | Port6 | Port7 | Port8 | Port9));  
      
      return true;
    }
    return false;
}

void loop()
{
  if (FoundDevice)
  {
    char stringBuf[20];
    int touchstatus = CypressCapsenseC8YC20.fetchTouchStatus(TestAddress);
    BINARYOUTPUT.int_to_binary_string(touchstatus, stringBuf);
    Serial.print("Touch Status: ");
    Serial.println(stringBuf);

    Serial.println("Raw Counts: ");
    for (int i = 0; i < 10; i++)
    {
        int ct = CypressCapsenseC8YC20.fetchRawCounts(TestAddress, Ports[i]);
        Serial.println("Port: " + String(i) + "(" + String( Ports[i]) + ") -------------- " + String(ct));
    }
    
    delay(1000);
  }
  
  light_loop();
}

void light_loop()
{
    // Fade in/fade out
  for(int j = 0; j < 3; j++ ) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int k = 0; k < 256; k++) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      delay(3);
    }
    for(int k = 255; k >= 0; k--) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      delay(3);
    }
  }
}

