using System;
using Microsoft.SPOT;
using System.IO.Ports;
using SecretLabs.NETMF.Hardware.Netduino;

namespace Helix
{
    class Serial_Communications
    {
        private const int __timeout__ = 20;

        public byte[] InBuffer;
        public byte[] OutBuffer;

        int m_InBufferSize;
        int m_OutBufferSize;

        SerialPort Serial;

        private enum handshake_codes
        {
            READY_TO_SEND = 1,
            RECEIVED_OK = 2,
            READY_TO_RECEIVE = 3,
            ERROR = 4
        };

        public Serial_Communications(int aInBufferSize, int aOutBufferSize)
        {
            InBuffer = new byte[aInBufferSize];
            OutBuffer = new byte[aOutBufferSize];

            m_InBufferSize = aInBufferSize;
            m_OutBufferSize = aOutBufferSize;
        }

        public void init()
        {
            Serial = new SerialPort(SerialPorts.COM1, 9600, Parity.None, 8, StopBits.One);
            Serial.Open();
        }

        // the overarching protocol.
        // There is no retry. If something fucks up, move on.
        // Netduino is the master, and initiates comms.
        public void communicate() // the overarching protocol
        {
            if (comm_error()) // Checking for proper init state.
            {
                dump_buffer("Comm Init Error");
                return;
            }

            if (handshake__send_data()) // Initiate communications. I want to send Arduino data
            {
                if (send_data()) // The data was sent. Yay!
                {
                    if (handshake__receive_data()) // Arduino wants to send me data.
                    {
                        if (!receive_data()) // I got the data! Yay!
                        {
                            dump_buffer("I failed to get the data.");
                            return;
                        }
                    }
                    else // Couldn't succeed in telling Netduino I want to send. Fuckit.
                    {
                        dump_buffer("Unable to coordinate expected data receipt with Arduino.");
                        return;
                    }
                }
                else // Some problem with receiving the data.
                {
                    dump_buffer("Problem sending the data to Arduino.");
                    return;
                }
            }
            else // Fuck!
            {
                dump_buffer("I failed to handshake for light data receipt.");
                return;
            }
        }

        // check for the whole thing being in a weird state
        bool comm_error()
        {
            if (Serial.BytesToRead == 0) // There shouldn't be ANYTHING here.
                return true;

            return false;
        }

        void dump_buffer(string aMessage)
        {
            Debug.Print("Comms Error: ");
            Debug.Print(aMessage);
            Debug.Print("");
            Debug.Print("Dumping buffer content to serial 0.");

            byte[] bytes = new byte[1];
            while (Serial.BytesToRead > 0)
            {
                Serial.Read(bytes, 0, bytes.Length);
                Debug.Print(bytes.ToString());
            }
        }

        bool waiting_for_data__with_timeout(int aTimeout) // timeout in ticks
        {
            long start = Microsoft.SPOT.Hardware.Utility.GetMachineTime().Ticks;
            while (Serial.BytesToRead <= 0) // wait for a response. -- TODO ADD A TIMEOUT
            {
                if (Microsoft.SPOT.Hardware.Utility.GetMachineTime().Ticks - start > aTimeout)
                    return false;
            }
            return true;
        }

        bool handshake__send_data()
        {
            byte[] outs = { (byte)handshake_codes.READY_TO_SEND };
            Serial.Write(outs, 0, outs.Length); // Send out initial handshake

            if (!waiting_for_data__with_timeout(__timeout__))
                return false;

            if (Serial.BytesToRead == 1) // no weird shit
            {
                byte[] handshake = new byte[1];
                Serial.Read(handshake, 0, handshake.Length);
                if (handshake[0] == (byte)handshake_codes.READY_TO_RECEIVE)
                {
                    return true;
                }
            }
            return false;
        }

        bool send_data()
        {
            Serial.Write(OutBuffer, 0, m_OutBufferSize);
            return true; // how can I possibly fail at this?
        }

        bool handshake__receive_data()
        {
            if (Serial.BytesToRead == 1)
            {
                byte[] handshake = new byte[1];
                Serial.Read(handshake, 0, handshake.Length);

                if (handshake[0] == (byte)handshake_codes.READY_TO_RECEIVE)
                {
                    byte[] outs = { (byte)handshake_codes.READY_TO_SEND };
                    Serial.Write(outs, 0, outs.Length); // reply

                    return true;
                }
            }
            return false;
        }

        bool receive_data()
        {
            if (!waiting_for_data__with_timeout(__timeout__))
                return false;

            Serial.Read(InBuffer, 0, InBuffer.Length);

            if (Serial.BytesToRead > 0)
            {                    
                return false;
            }
            return true;
        }
    }
}
