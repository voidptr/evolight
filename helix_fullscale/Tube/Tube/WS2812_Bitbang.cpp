
#include "WS2812_Bitbang.h"

#define INTERRUPTS interrupts();
#define NOINTERRUPTS noInterrupts();
#define NOT_A_PIN NULL

// Some ASM defines
#if defined(__arm__) 
#  define NOP __asm__ __volatile__ ("nop\n");
#elif defined(__PIC32MX__)
#  define NOP __asm__ __volatile__ ("nop\n");
#else
#  define NOP __asm__ __volatile__ ("cp r0,r0\n");
#endif

#define NOP_25NS NOP; NOP;
#define NOP_50NS NOP_25NS; NOP_25NS;
#define NOP_100NS NOP_50NS; NOP_50NS;
#define NOP_200NS NOP_100NS; NOP_100NS;
#define NOP_250NS NOP_200NS; NOP_50NS;
#define NOP_450NS NOP_250NS; NOP_200NS;
#define NOP_500NS NOP_250NS; NOP_250NS;
#define NOP_800NS NOP_500NS; NOP_100NS; NOP_200NS;
#define NOP_1000NS NOP_500NS; NOP_500NS;

#define NOP_SHORT_NS NOP_200NS
#define NOP_LONG_NS NOP_1000NS
#define NOP_MEDIUM_NS NOP_800NS

// TODO: Better: MASH_HI(_PORT,PINMASK); NOP; NOP; MASK_SET(_PORT, PINMASK, X & (1<<N)); NOP; NOP; MASK_LO(_PORT, PINMASK); (loop logic)
// TODO: Best - interleave X lines

#define MASK_HI(R_port, P_pinmask) (R_port)->lat.set = (P_pinmask)
#define MASK_LO(R_port, P_pinmask) (R_port)->lat.clr = (P_pinmask)

/* ---------- DEFINES for setting a single pin at a time ---------- */
// send a bit
#define TM1809_BIT_SET(X,N,_PORT)  \
    if( X & (1<<N) ) { \
       MASK_HI(_PORT,PINMASK); \
       NOP_LONG_NS; \
       MASK_LO(_PORT,PINMASK); \
       NOP_SHORT_NS; \
    } else { \
       MASK_HI(_PORT,PINMASK); \
       NOP_SHORT_NS; \
       MASK_LO(_PORT,PINMASK); \
       NOP_LONG_NS; \
    }

// send a byte
#define TM1809_BIT_ALL(_PORT)   \
    TM1809_BIT_SET(x,7,_PORT); \
    TM1809_BIT_SET(x,6,_PORT); \
    TM1809_BIT_SET(x,5,_PORT); \
    TM1809_BIT_SET(x,4,_PORT); \
    TM1809_BIT_SET(x,3,_PORT); \
    TM1809_BIT_SET(x,2,_PORT); \
    TM1809_BIT_SET(x,1,_PORT); \
    TM1809_BIT_SET(x,0,_PORT);

// send all the bytes
#define TM1809_ALL(_PORT,PTR, END) \
    while(PTR != END) { \
        register unsigned char x = *PTR++;  \
        TM1809_BIT_ALL(_PORT); \
        x = *PTR++; \
        TM1809_BIT_ALL(_PORT); \
        x = *PTR++; \
        TM1809_BIT_ALL(_PORT); \
    }


/* ---------- DEFINES for setting a two pins on a port at a time ---------- */
#define TM1809_BIT_SET_2FER(X,X2,N,_PORT) \
if ((X & 1<<N) && (X2 & 1<<N)) { \
MASK_HI(_PORT,(PINMASK|PINMASK2)); \
NOP_LONG_NS; \
MASK_LO(_PORT,(PINMASK|PINMASK2)); \
NOP_SHORT_NS; \
} \
else if ((X & 1<<N) && !(X2 & 1<<N)) { \
MASK_HI(_PORT,(PINMASK|PINMASK2)); \
NOP_SHORT_NS; \
MASK_LO(_PORT,PINMASK2); \
NOP_MEDIUM_NS; \
MASK_LO(_PORT,PINMASK); \
NOP_SHORT_NS; \
} \
else if ((X2 & 1<<N) && !(X & 1<<N)) { \
MASK_HI(_PORT,(PINMASK|PINMASK2)); \
NOP_SHORT_NS; \
MASK_LO(_PORT,PINMASK); \
NOP_MEDIUM_NS; \
MASK_LO(_PORT,PINMASK2); \
NOP_SHORT_NS; \
} \
else { \
MASK_HI(_PORT,(PINMASK|PINMASK2)); \
NOP_SHORT_NS; \
MASK_LO(_PORT,(PINMASK|PINMASK2)); \
NOP_LONG_NS; \
}

// send a byte, two strands at a time
#define TM1809_BIT_ALL_2FER(_PORT)   \
TM1809_BIT_SET_2FER(x,x2,7,_PORT); \
TM1809_BIT_SET_2FER(x,x2,6,_PORT); \
TM1809_BIT_SET_2FER(x,x2,5,_PORT); \
TM1809_BIT_SET_2FER(x,x2,4,_PORT); \
TM1809_BIT_SET_2FER(x,x2,3,_PORT); \
TM1809_BIT_SET_2FER(x,x2,2,_PORT); \
TM1809_BIT_SET_2FER(x,x2,1,_PORT); \
TM1809_BIT_SET_2FER(x,x2,0,_PORT);

// send all the bytes, two strands at a time
#define TM1809_ALL_2FER(_PORT,PTR, END, PTR2, END2) \
    while(PTR != END) { \
        register unsigned char x = *PTR++;  \
        register unsigned char x2 = *PTR2++;  \
        TM1809_BIT_ALL_2FER(_PORT); \
        x = *PTR++; \
        x2 = *PTR2++; \
        TM1809_BIT_ALL_2FER(_PORT); \
        x = *PTR++; \
        x2 = *PTR2++; \
        TM1809_BIT_ALL_2FER(_PORT); \
    }

/* ---------- DEFINES for setting a eight pins at a time ---------- */
#define TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,N,_PORT) \
    MASK_HI(_PORT, combined_pinmask); \
    NOP_SHORT_NS; \
    if ((DATA[0] & 1<<N)) { MASK_HI(_PORT, pinmasks[0]); } else { MASK_LO(_PORT, pinmasks[0]); } \
    if ((DATA[1] & 1<<N)) { MASK_HI(_PORT, pinmasks[1]); } else { MASK_LO(_PORT, pinmasks[1]); } \
    if ((DATA[2] & 1<<N)) { MASK_HI(_PORT, pinmasks[2]); } else { MASK_LO(_PORT, pinmasks[2]); } \
    if ((DATA[3] & 1<<N)) { MASK_HI(_PORT, pinmasks[3]); } else { MASK_LO(_PORT, pinmasks[3]); } \
    if ((DATA[4] & 1<<N)) { MASK_HI(_PORT, pinmasks[4]); } else { MASK_LO(_PORT, pinmasks[4]); } \
    if ((DATA[5] & 1<<N)) { MASK_HI(_PORT, pinmasks[5]); } else { MASK_LO(_PORT, pinmasks[5]); } \
    if ((DATA[6] & 1<<N)) { MASK_HI(_PORT, pinmasks[6]); } else { MASK_LO(_PORT, pinmasks[6]); } \
    if ((DATA[7] & 1<<N)) { MASK_HI(_PORT, pinmasks[7]); } else { MASK_LO(_PORT, pinmasks[7]); } \
    NOP_MEDIUM_NS; \
    MASK_LO(_PORT, combined_pinmask); \
    NOP_SHORT_NS;

#define TM1809_BIT_ALL_PARALLEL(_PORT, DATA, combined_pinmask)   \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,7,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,6,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,5,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,4,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,3,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,2,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,1,_PORT); \
TM1809_BIT_SET_PARALLEL(DATA, combined_pinmask,0,_PORT);


void WS2812_Bitbang::show_parallel() {
    
    //NOINTERRUPTS
    
    int lPinCount = 1;
    int lLongestPin = 0;
    int lLongestData = 0;
    byte * lLongestDataEnd;
    
    byte * pRunningData = m_pData; // initialize this to the start of the data set
    
    // try doing as many as possible at a time
    for(int iPins = 0; iPins < m_nPins; iPins++) {
        
        uint32_t pinmasks[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t combined_pinmask = 0;
        byte * pData[8] = {0, 0, 0, 0 ,0, 0, 0, 0};
        
        uint8_t port = digitalPinToPort(m_pPins[iPins]);
        
        if(port == NOT_A_PIN)
        {
            continue; // skip this pin
        }
        volatile p32_ioport * iop = (p32_ioport *)portRegisters(port);
        
        // figure out which pins we will do, up to a max of 8
        for (int i = 1; (i < 8) && (iPins < m_nPins); i++)
        {
            uint8_t nextPort = digitalPinToPort(m_pPins[iPins + i]);
            
            if (((iPins + i) < m_nPins) && // there's a pin here
                (nextPort == port) && // it's the same port (!)
                (nextPort != NOT_A_PIN)) // and it's a real pin
            { // ok, let's do it
                lPinCount++;
            }
            else
                break;
        }        
        
        // populate the table appropriately
        for (int i = 0; i < lPinCount; i++)
        {
            pinmasks[i] = digitalPinToBitMask(m_pPins[iPins + i]);
            combined_pinmask |= pinmasks[i];
            pData[i] = pRunningData;
            pRunningData += m_pPinLengths[iPins + i]; // move it to the next segment.
            
            if (lLongestData < m_pPinLengths[iPins + i])
            {
                //lLongestDataP = pData[i];
                lLongestData = m_pPinLengths[iPins + i];
                lLongestPin = i;
                lLongestDataEnd = pRunningData; // mark the end
            }
        }
        iPins += lPinCount - 1;
        
        byte lData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        Debug::debugln("Starting!");
        Debug::debug(iPins - (lPinCount - 1));
        Debug::debug(" - ");
        Debug::debugln(lPinCount);
        Debug::debugln(lLongestPin);
        Debug::debug_binary((uint32_t)pData[lLongestPin]);
        Debug::debugln();
        delay(1000);
        //NOINTERRUPTS
        
        //for (int q = 0; q < 16; q++) {
        while(pData[lLongestPin] != lLongestDataEnd) {
            //Debug::debug_binary((uint32_t)pData[lLongestPin]);
            //Debug::debug(" -> ");
            //Debug::debug_binary((uint32_t)lLongestDataEnd);
            //Debug::debugln();
            for (int i = 0; i < 8; i++) {
                lData[i] = *(pData[i]++);
            }
        //    TM1809_BIT_ALL_PARALLEL(iop, lData, combined_pinmask);
            for (int i = 0; i < 8; i++) {
                lData[i] = *(pData[i]++);
            }
        //    TM1809_BIT_ALL_PARALLEL(iop, lData, combined_pinmask);
            for (int i = 0; i < 8; i++) {
                lData[i] = *(pData[i]++);
            }
        //    TM1809_BIT_ALL_PARALLEL(iop, lData, combined_pinmask);
            //lLongestDataP += 3; // move up 3
        } 
        //INTERRUPTS
        Debug::debugln("Done!");        
    }
    //INTERRUPTS
}

void WS2812_Bitbang::show() {
    
    NOINTERRUPTS
    
    byte *pData = m_pData;
    byte *pEnd;
    int iPins2;
    
    uint32_t PINMASK;
    uint8_t port;
    volatile p32_ioport * iop;
    
    // try doing two at a time
    for(int iPins = 0; iPins < m_nPins; iPins++) {

        pEnd = pData + m_pPinLengths[iPins];
        iPins2 = iPins + 1;
        
        PINMASK = digitalPinToBitMask(m_pPins[iPins]);
        port = digitalPinToPort(m_pPins[iPins]);
        iop = (p32_ioport *)portRegisters(port);

        if(port == NOT_A_PIN)
        {
            continue; // skip this pin
        }
        
        uint8_t port2 = digitalPinToPort(m_pPins[iPins2]);
        if(false)
//        if ((iPins2 < m_nPins) && // there's a pin here
//            (port2 == port) && // it's the same port (!)
//            (port2 != NOT_A_PIN)) // and it's a real pin
        { // ok, let's do 2 at a time
            register byte *pData2 = pEnd; // grab a pointer to the second stack of data
            register byte *pEnd2 = pData2 + m_pPinLengths[iPins2]; // and a pointer to the end of it
            uint32_t PINMASK2 = digitalPinToBitMask(m_pPins[iPins2]);
            
            // organize them so the longer of them goes first. fucking weird code unroll, and I'm too dumb.            
            if (m_pPinLengths[iPins] > m_pPinLengths[iPins2])
            {
                TM1809_ALL_2FER(iop, pData, pEnd, pData2, pEnd2);
            }
            else
            {
                TM1809_ALL_2FER(iop, pData2, pEnd2, pData, pEnd);
            }
            
            iPins++; // increment past the second pin
        }
        else
        {
            TM1809_ALL(iop, pData, pEnd);
        }
    }
    INTERRUPTS
}




