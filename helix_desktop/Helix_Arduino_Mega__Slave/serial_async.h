#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

#ifndef __TIMEOUT__
    #define __TIMEOUT__ 600
#endif

#ifndef __BAUD__
    #define __BAUD__ 9600
#endif

#ifndef __PACKET_SIZE__
    #define __PACKET_SIZE__ 5
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).

struct packet 
{ 
    int light; 
    byte r; 
    byte g; 
    byte b; 

    packet(byte * reads)
    {
        light = (reads[0] << 8) | reads[1];
        r = reads[2];
        g = reads[3];
        b = reads[4];
    }
};

class Serial_Communications 
{
public:
//    struct packet { int light; byte r; byte g; byte b; };
    
    int m_BytesRead;

    byte InBuffer[__TOTAL_LIGHT_COUNT__ * __PACKET_SIZE__];

private:
   
    byte OutBuffer[__PAD_COUNT__]; // outgoing

    int m_OutBufferSize;
    int m_InBufferSize;

public:
    // constructor, pass in the basic crap so we can pass it on to our minions.
    Serial_Communications() 
    {
        m_OutBufferSize = __PAD_COUNT__;
        m_InBufferSize = __TOTAL_LIGHT_COUNT__ * __PACKET_SIZE__;

        m_BytesRead = 0;
    }

    void init() 
    { 
        // init the serial
        Serial1.begin(__BAUD__);    
    }

    void pull_data()
    {
        int ct = 0;
        int lTimeout = 0;
        m_BytesRead = 0;
        while ( lTimeout < 2 )
        {
            while ( Serial1.available() > 0 ) // more to read?
            {
                InBuffer[m_BytesRead++] = Serial1.read();
        
                delay(1);
            }
            lTimeout++;
            delay(20);
            if ( Serial1.available() > 0 )
            {
                lTimeout = 0;
            }
        }
        debug(m_BytesRead);
    }

    bool dump_buffer(char * aMessage)
    {
        debug("Comms Error: ");
        debug(aMessage);
        debug("");
        debug("Dumping buffer content to serial 0.");

        while( Serial1.available() )
        {
            Serial1.read();
        }
    }

    bool send_data()
    {
        debug("send_data() -- DDD --");
        debug("--- SENDING DDD DATA 6 ---");
        for (int i = 0; i < m_OutBufferSize; i++)
        {
            Serial1.write(OutBuffer[i]);
        }
        debug("YEP! --- SENT DDD DATA 6 ---");
        return true; // how can I possibly fail at this?
    }
};
    


