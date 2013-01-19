#include <IOShieldOled.h>

#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class OLED_IO 
{
private:

  uint8_t buffer[128]; // one buffer line
  int poop;
public:
  // constructor, pass in the basic crap so we can pass it on to our minions.
  OLED_IO() 
  {
    //debug("OLED_IO CTOR");
    poop = 0;

    //debug("END OLED_IO CTOR");
  }

  void init() 
  {
    debug("  OLED_IO init()");

    delay(1000);
    debug("Starting IOShiled");

    IOShieldOled.begin();

    debug("  END OLED_IO init()");
  }

  // do the output
  void output() 
  {
    //reset_display();
    debug("START output()");
    debug(poop);
    debug_oled(String(poop++));
    //    slide_rectangle_with_writing();
    //    blink_display(3);
    //    clear_display();
    //    display_test_patterns();
    //    populate_lights_array();
    debug("END output()");
  }


private:

  void scroll_up()
  {
    debug("START scroll_up()");
    // row 2 to row 1
    IOShieldOled.moveTo(8, 0);
    IOShieldOled.getBmp(128, 8, buffer);
    IOShieldOled.moveTo(0, 0);
    IOShieldOled.putBmp(128, 8, buffer);

    debug(String((char *)buffer));

    // row 3 to row 2
    IOShieldOled.moveTo(16, 0);
    IOShieldOled.getBmp(128, 8, buffer);
    IOShieldOled.moveTo(8, 0);
    IOShieldOled.putBmp(128, 8, buffer);

    //debug(String(buffer));

    // row 4 to row 3
    IOShieldOled.moveTo(24, 0);
    IOShieldOled.getBmp(128, 8, buffer);
    IOShieldOled.moveTo(16, 0);
    IOShieldOled.putBmp(128, 8, buffer);

    //debug(String(buffer));

    //IOShieldOled.moveTo(24, 0);
    //IOShieldOled.drawFillRect(128, 8);

    IOShieldOled.updateDisplay();
    debug("END scroll_up()");
  }



  void debug_oled(String aString)
  {
    output_string_to_oled(aString);
  }

  void output_string_to_oled(String aString)
  {
    debug("START output_string_to_oled()");
    int cursX = 0, cursY = 0;
    IOShieldOled.getCursor(&cursX, &cursY);

    debug("cursor: X:" + String(cursX) + " Y:" + String(cursY));

    if (cursY > 2 || (cursY == 0 && cursX == 0)) // we're already on the fourth line, or we wrapped around to the beginning. oops.
    {
      debug("Srolling up! (on line 3)");
      scroll_up();
      IOShieldOled.setCursor(0, 3);
    }
    else if (cursX > 0) // aren't on a fresh new line. carriage return.
    {
      debug("Carriage Return");
      IOShieldOled.setCursor(0, cursY + 1);
    }
    


    char charBuf[17];
    int len = (aString.length() < 17) ? aString.length() : 16;

    debug("string (" + aString + ") length: " + String(aString.length()) + " len: " + String(len));

    aString.toCharArray(charBuf, len + 1);
    charBuf[len] = '\0'; // if there isn't one already

    debug("string (" + aString + ") length: " + String(aString.length()) + " len: " + String(len) + " results: " + String(charBuf));

    IOShieldOled.putString(charBuf); // ok, write the line

    if (aString.length() > 16) // more left?
    {
      output_string_to_oled(aString.substring(16));
    }
    debug("END output_string_to_oled()");
  }


  void reset_display()
  {
    //Clear the virtual buffer
    IOShieldOled.clearBuffer();

    //Chosing Fill pattern 0
    IOShieldOled.setFillPattern(IOShieldOled.getStdPattern(0));
    //Turn automatic updating off
    //IOShieldOled.setCharUpdate(0);
  }

  void slide_rectangle_with_writing()
  {
    //Draw a rectangle over wrting then slide the rectagle
    //down slowly displaying all writing
    for (int irow = 0; irow < IOShieldOled.rowMax; irow++)
    {
      IOShieldOled.clearBuffer();
      IOShieldOled.setCursor(0, 0);
      IOShieldOled.putString("HELIX");
      IOShieldOled.setCursor(0, 1);
      IOShieldOled.putString("Debug Output");
      IOShieldOled.setCursor(0, 2);
      IOShieldOled.putString("by RCK");

      IOShieldOled.moveTo(0, irow);
      IOShieldOled.drawFillRect(127,31);
      IOShieldOled.moveTo(0, irow);
      IOShieldOled.drawLine(127,irow);
      IOShieldOled.updateDisplay();
      delay(10);
    }
  }

  void blink_display( int aCount )
  {
    for (int i = 0; i < aCount; i++)
    {
      IOShieldOled.displayOff();
      delay(500);
      IOShieldOled.displayOn();
      delay(500);
    }
  }

  void clear_display()
  {
    // Now erase the characters from the display
    for (int irow = IOShieldOled.rowMax-1; irow >= 0; irow--) {
      IOShieldOled.setDrawColor(1);
      IOShieldOled.setDrawMode(IOShieldOled.modeSet);
      IOShieldOled.moveTo(0,irow);
      IOShieldOled.drawLine(127,irow);
      IOShieldOled.updateDisplay();
      delay(25);
      IOShieldOled.setDrawMode(IOShieldOled.modeXor);
      IOShieldOled.moveTo(0, irow);
      IOShieldOled.drawLine(127, irow);
      IOShieldOled.updateDisplay();
    }
  }

  void display_test_patterns()
  {
    IOShieldOled.setDrawMode(IOShieldOled.modeSet);

    for(int ib = 1; ib < 8; ib++)
    {
      IOShieldOled.clearBuffer();

      IOShieldOled.setFillPattern(IOShieldOled.getStdPattern(ib));
      IOShieldOled.moveTo(55, 1);
      IOShieldOled.drawFillRect(75, 27);
      IOShieldOled.drawRect(75, 27);
      IOShieldOled.updateDisplay();

      delay(1000);
    }
  }

};







