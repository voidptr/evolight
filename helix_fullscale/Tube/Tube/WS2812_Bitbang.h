/*
 WS2812_Bitbang.h - Controller for World-Semi WS2812 integrated PWM and RGB LED
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
 Controller for World-Semi WS2812 integrated PWM and RGB LED
 Developed for the ChipKit microcontroller boards by Digilent Inc.
 
 Library based on FastSPI_LED, Copyright (c) 2010 Daniel Garcia.
 http://code.google.com/p/fastspi/
 
 Timings and ChipKit register code based on code by Brian Chojnowski
 http://myroundpeg.blogspot.com
*/

#ifndef WS2812_BITBANG_h
#define WS2812_BITBANG_h

// Core library for code-sense
#include "IDEIncludes.h"
#include "debug.h"

/* ----- DEFAULTS. Set these values in your sketch ----- */
//#ifndef __NUM_STRANDS__
//#define __NUM_STRANDS__ 1
//#endif

//#ifndef __LIGHT_PINS__
//#define __LIGHT_PINS__ {4} // 4-13 organisms, 14 status lights
//#endif

//#ifndef __LIGHTS_AT_PINS__
//#define __LIGHTS_AT_PINS__ {30}
//#endif
/* ----- END DEFAULTS ----- */

class WS2812_Bitbang {
public:

    int m_nLeds;
    unsigned char m_pData[__TOTAL_LIGHTS__ * 3];
    unsigned char *m_pDataEnd;
    unsigned int m_pPins[__NUM_STRANDS__];
    unsigned int m_pPinLengths[__NUM_STRANDS__];
    unsigned int m_nPins;
    uint8_t *m_pPorts[__NUM_STRANDS__];

    WS2812_Bitbang() {
        m_nPins = __NUM_STRANDS__;
        
        m_pPins = __LIGHT_PINS__;
        
        unsigned char tmpPinLengths[__NUM_STRANDS__] = __LIGHTS_AT_PINS__;
        
        for(int i = 0; i < m_nPins; i++) {
            m_pPinLengths[i] = tmpPinLengths[i] * 3;
            m_pPorts[i] = (uint8_t*)portOutputRegister(digitalPinToPort(m_pPins[i]));
        }
        
        m_nLeds = __TOTAL_LIGHTS__ * 3;
        memset(m_pData,0,m_nLeds);
        m_pDataEnd = m_pData + m_nLeds;
    }
    
    // initialize the pins
    void begin() {
        for(int i = 0; i < m_nPins; i++) {
            pinMode(m_pPins[i], OUTPUT);
            digitalWrite(m_pPins[i],LOW);
        }
    }
    
    // call this method to get a pointer to the raw rgb data
    unsigned char *getRGBData() { return m_pData; }
       
    // 'show' or push the current led data 
    void show();
    void show_parallel();

};

#endif

