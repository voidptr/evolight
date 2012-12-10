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

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class Serial_Communications 
{
public:

    byte InBuffer[__TOTAL_LIGHT_COUNT__ * 3]; // incoming
    byte OutBuffer[__PAD_COUNT__]; // outgoing

    int m_InBufferSize;
    int m_OutBufferSize;

private:

    enum 
    {
        READY_TO_SEND = 1,
        RECEIVED_OK = 2,
        READY_TO_RECEIVE = 3,
        ERROR = 4
    };

public:
    // constructor, pass in the basic crap so we can pass it on to our minions.
    Serial_Communications() 
    {
        m_InBufferSize = __TOTAL_LIGHT_COUNT__ * 3;
        m_OutBufferSize = __PAD_COUNT__;
    }

    void init() 
    { 
        // init the serial
        Serial1.begin(__BAUD__);    
    }

    // the overarching protocol.
    // There is no retry. If something fucks up, move on.
    void communicate() // the overarching protocol
    {
        debug("communicate()");
        
        if (!comm_has_data()) // for whatever reason, there was nothing here. Shrug.        
            return;

       
        if (comm_error()) // Checking for proper init state.
        {
            dump_buffer("Comm Init Error");
            return;
        }

        
        if (handshake__receive_data()) // Netduino wants to send me data
        {
            
            if (receive_data()) // The data was received. Yay!
            {
                
                if (handshake__send_data()) // I want to send Netduino data.
                {
                    
                    if (!send_data()) // I sent the data! Yay!
                    {
                        dump_buffer("I failed to send Netduino the data.");
                        return;
                    }
                } 
                else // Couldn't succeed in telling Netduino I want to send. Fuckit.
                {
                    dump_buffer("Unable to tell Netduino I want to send.");
                    return;
                }
            }
            else // Some problem with receiving the data.
            {
                dump_buffer("Problem receiving the data from Netduino.");
                return;
            }
        }
        else // Netduino is incomprehensible.
        {
            dump_buffer("Netduino failed to handshake for light data receipt.");
            return;
        }
        debug("END communicate()");
    }

    // check whether there is ANYTHING in the buffer.
    bool comm_has_data()
    {
        debug("comms_has_data() --- 0a --- ");
        if ( Serial1.available() < 1 )
        {
            debug("Nope.");
            return false;
        }
        
        debug("Yep!");
        debug(Serial1.available());
        return true;
    }

    // check for the whole thing being in a weird state
    bool comm_error()
    {
        debug("comms_error() --- 0b --- ");
        if ( Serial1.available() != 1 ) // there should only be one thing here.
        {
            debug("Yep!");
            return true;
        }
        debug("Nope.");
        return false;
    }

    bool dump_buffer(char * aMessage)
    {
        debug("Comms Error: ");
        debug(aMessage);
        debug("");
        debug("Dumping buffer content to serial 0.");

        while( Serial1.available() )
        {
            debug(Serial1.read());
        }
    }

    bool handshake__receive_data()
    {
        debug("handshake__receive_data() --- AAA ---");
        if (!waiting_for_data__with_timeout(__TIMEOUT__))
        {
            debug("TIMEOUT! __no aaa call 1__");
            return false;
        }

        if ( Serial1.available() == 1 )
        {
            int handshake = Serial1.read();
            if (handshake == READY_TO_SEND) 
            {
                debug("Yep! --- RCVD AAA CALL 1 ---");
                Serial1.write(READY_TO_RECEIVE); // reply
                debug("--- SEND AAA RESPONSE 2 ---");
                return true;
            }            
        }
        debug("Nope __bad aaa call 1__");
        return false;
    }
    
    bool waiting_for_data__with_timeout(int aTimeout) // timeout in miliseconds
    {
        unsigned long start = millis();
        while ( Serial1.available() <= 0 ) // wait for a response. -- TODO ADD A TIMEOUT
        {
            if (millis() - start > aTimeout)
                return false;
        }
        return true;
    }

    bool receive_data()
    {
        debug("receive_data() --- BBB ---");

        int lCount = 0;
        int lTimeouts = 0;
        while (lCount < m_InBufferSize)
        {
            if (!waiting_for_data__with_timeout(__TIMEOUT__))
            {
                lTimeouts++;
            }
            else
            {
                byte inByte = Serial1.read();
                if (lCount < m_InBufferSize)
                {
                    InBuffer[lCount++] = inByte;
                }
                else
                {
                    debug("NOPE! __bad bbb data 3__");
                    return false;
                }
            }

            if (lTimeouts > 10)
            {
                debug("TIMED OUT DURING RECEIVE __did not receive bbb data 3__ ");
                debug("Received: ");
                debug(lCount);
                debug("Out of: ");
                debug(m_InBufferSize);
                return false;
            }

        }
        debug("Received: ");
        debug(lCount);
        debug("Yep! --- RECV BBB DATA 3 ---");
        return true;
    }

    bool handshake__send_data()
    {
        debug("handshake__send_data() --- CCC ---");
        Serial1.write(READY_TO_SEND);
        debug(" --- SEND CALL CCC 4 ---");

        if (!waiting_for_data__with_timeout(__TIMEOUT__))
        {
            debug("TIMEOUT! __no ccc response 5__");
            return false;
        }

        if ( Serial1.available() == 1 ) // weird shit
        {
            byte handshake = Serial1.read();
            if (handshake == READY_TO_RECEIVE) 
            {
               debug("YEP! --- RCVD CCC RESPONSE 5 ---");
               return true;
            }
        }
        debug("NOPE! __bad ccc response 5__");
        return false;
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
    


