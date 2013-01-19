#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include <pins_arduino.h>
#else
#include "WProgram.h"
#include <pins_arduino.h>
#include "wiring.h"
#include <HardwareSerial.h>
#include <p32_defs.h>
#endif

#include "FastSPI_LED_Chipkit_Minimal.h"

#define DEBUG_SPI
#ifdef DEBUG_SPI
#  define DPRINT Serial.print
#  define DPRINTLN Serial.println
#else
#  define DPRINT(x)
#  define DPRINTLN(x)
#endif

// #define COUNT_ROUNDS 1

// definitions for ARM vs whatever else
//#if defined(_VARIANT_ARDUINO_DUE_X_)
#define SPI_AVAILABLE 0
#define INTERRUPTS interrupts();
#define NOINTERRUPTS noInterrupts();
#define NOT_A_PIN NULL
//#else
//#define SPI_AVAILABLE 1
//#define INTERRUPTS sei();
//#define NOINTERRUPTS cli();
//#endif

//#define _BV(bit) (1 << (bit)) // byte value

#define BIT_HI(R, P) (R) |= _BV(P)
#define BIT_LO(R, P) (R) &= ~_BV(P)

#define MASK_HI(R, P) (R)->lat.set = (P)
#define MASK_LO(R, P) (R)->lat.clr = (P)

//#define MASK_HI(R, P) (R) |= (P)
//#define MASK_LO(R, P) (R) &= ~(P)

// HL1606 defines
//Commands for each LED to be ORd together.

#define TM_1606 153

#define Command     B10000000
#define Commandx2   B11000000   //Use this one to make dimming twice as fast.
#define BlueOff    B00000000   
#define BlueOn     B00010000
#define BlueUp     B00100000
#define BlueDown   B00110000
#define RedOff      B00000000
#define RedOn       B00000100
#define RedUp       B00001000
#define RedDown     B00001100
#define GreenOff     B00000000
#define GreenOn      B00000001
#define GreenUp      B00000010
#define GreenDown    B00000011

#define BRIGHT_MAX 256

// Some ASM defines
#if defined(__arm__) 
#  define NOP __asm__ __volatile__ ("nop\n");
#elif defined(__PIC32MX__)
#  define NOP __asm__ __volatile__ ("nop\n");
#else
#  define NOP __asm__ __volatile__ ("cp r0,r0\n");
#endif

#define NOP2 NOP NOP
#define NOP1 NOP
#define NOP3 NOP NOP2
#define NOP4 NOP NOP3
#define NOP5 NOP NOP4
#define NOP6 NOP NOP5
#define NOP7 NOP NOP6
#define NOP8 NOP NOP7
#define NOP9 NOP NOP8
#define NOP10 NOP NOP9
#define NOP11 NOP NOP10
#define NOP12 NOP NOP11
#define NOP13 NOP NOP12
#define NOP14 NOP NOP13
#define NOP15 NOP10 NOP5
#define NOP16 NOP14 NOP2
#define NOP20 NOP10 NOP10
#define NOP22 NOP20 NOP2

#define NOP_SHORT NOP2
#define NOP_LONG NOP5

#define NOP_25NS NOP; NOP;
#define NOP_50NS NOP_25NS; NOP_25NS;
#define NOP_100NS NOP_50NS; NOP_50NS;
#define NOP_200NS NOP_100NS; NOP_100NS;
#define NOP_250NS NOP_200NS; NOP_50NS;
#define NOP_450NS NOP_250NS; NOP_200NS;
#define NOP_500NS NOP_250NS; NOP_250NS;
#define NOP_1000NS NOP_500NS; NOP_500NS;

#define NOP_SHORT_NS NOP_200NS
#define NOP_LONG_NS NOP_1000NS

// TODO: Better: MASH_HI(_PORT,PINMASK); NOP; NOP; MASK_SET(_PORT, PINMASK, X & (1<<N)); NOP; NOP; MASK_LO(_PORT, PINMASK); (loop logic)
// TODO: Best - interleave X lines 
#define TM1809_BIT_SET(X,N,_PORT) if( X & (1<<N) ) { MASK_HI(_PORT,PINMASK); NOP_LONG_NS; MASK_LO(_PORT,PINMASK); NOP_SHORT_NS; } else { MASK_HI(_PORT,PINMASK); NOP_SHORT_NS; MASK_LO(_PORT,PINMASK); NOP_LONG_NS; }

#define TM1809_BIT_ALL(_PORT)   \
    TM1809_BIT_SET(x,7,_PORT); \
TM1809_BIT_SET(x,6,_PORT); \
TM1809_BIT_SET(x,5,_PORT); \
TM1809_BIT_SET(x,4,_PORT); \
TM1809_BIT_SET(x,3,_PORT); \
TM1809_BIT_SET(x,2,_PORT); \
TM1809_BIT_SET(x,1,_PORT); \
TM1809_BIT_SET(x,0,_PORT);

#define TM1809_ALL(_PORT,PTR, END) \
    while(PTR != END) { register unsigned char x = *PTR++;  TM1809_BIT_ALL(_PORT); \
        x = *PTR++; TM1809_BIT_ALL(_PORT); \
        x = *PTR++; TM1809_BIT_ALL(_PORT); }

#define NOP_SHORT_1903 NOP2
#define NOP_LONG_1903 NOP15

#define TM1809_BIT_ALLD TM1809_BIT_ALL(PORTD);
#define TM1809_BIT_ALLB TM1809_BIT_ALL(PORTB);

CFastSPI_LED FastSPI_LED;

// local prototyps
extern "C" { 
    void spi595(void);
    void spihl1606(void);
    void spilpd6803(void);
};

// local variables used for state tracking and pre-computed values
// TODO: move these into the class as necessary
static unsigned char *pData;
static unsigned char nBrightIdx=0;
static unsigned char nBrightMax=0;
static unsigned char nCountBase=0;
static unsigned char nCount=0;
static unsigned char nLedBlocks=0;
unsigned char nChip=0;
//static unsigned long adjustedUSecTime;

void CFastSPI_LED::setDirty() { m_nDirty = 1; }

void CFastSPI_LED::init() { 
    // store some static locals (makes lookup faster)
    pData = m_pDataEnd;
    nCountBase = m_nLeds / 3;
    // set up the spi timer - also do some initial timing loops to get base adjustments
    // for the timer below  
    setup_hardware_spi();
}

// 
void CFastSPI_LED::start() {
}

void CFastSPI_LED::stop() {
}


void CFastSPI_LED::setChipset(EChipSet eChip) {
    m_eChip = eChip;
    nChip = eChip;

    m_cpuPercentage = 5;
    m_nDataRate = 0;
}

void CFastSPI_LED::show() { 
    static byte run=0;

    setDirty();
    NOINTERRUPTS
        m_nDirty = 0;
    register byte *pData = m_pData;
    for(int iPins = 0; iPins < m_nPins; iPins++) { 

        register byte *pEnd = pData + m_pPinLengths[iPins];

        uint32_t PINMASK = digitalPinToBitMask(FastSPI_LED.m_pPins[iPins]);
        uint8_t port = digitalPinToPort(FastSPI_LED.m_pPins[iPins]);
        volatile p32_ioport * iop = (p32_ioport *)portRegisters(port);

        if(port == NOT_A_PIN) 
        { /* do nothing */ } 
        else 
        { 
            TM1809_ALL(iop, pData, pEnd); 
        }

    }
    INTERRUPTS
}

void CFastSPI_LED::setDataRate(int datarate) {
    m_nDataRate = datarate;
}

void CFastSPI_LED::setPinCount(int nPins) {
    m_nPins = nPins;
    m_pPins = (unsigned int*)malloc(sizeof(unsigned int) * nPins);
    m_pPinLengths = (unsigned int*)malloc(sizeof(unsigned int) * nPins);
    m_pPorts = (uint8_t**)malloc(sizeof(uint8_t*) * nPins);
    for(int i = 0; i < nPins; i++) { 
        m_pPins[i] = m_pPinLengths[i] = 0;
        m_pPorts[i] = NULL;
    }
}

void CFastSPI_LED::setPin(int iPins, int nPin, int nLength) {
    m_pPins[iPins] = nPin;
    m_pPinLengths[iPins] = nLength*3;
    m_pPorts[iPins] = (uint8_t*)portOutputRegister(digitalPinToPort(nPin));
}

void CFastSPI_LED::setup_hardware_spi(void) {
    byte clr;
    for(int i = 0; i < m_nPins; i++) { 
        
        pinMode(m_pPins[i], OUTPUT);
        digitalWrite(m_pPins[i],LOW);
    }

}



