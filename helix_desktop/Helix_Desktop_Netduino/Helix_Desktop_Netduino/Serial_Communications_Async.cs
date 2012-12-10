using System;
using Microsoft.SPOT;
using System.IO.Ports;
using SecretLabs.NETMF.Hardware.Netduino;
using System.Threading;

namespace Helix
{
    class Serial_Communications_Async
    {
        private const int __baud__ = 9600;

        SerialPort Serial;
        public byte[] OutBuffer;
        private int OutBufferSize;
        private int Count;

        private const int __burst_size__ = 300;

        public Serial_Communications_Async(int aOutBufferSize)
        {
            OutBuffer = new byte[aOutBufferSize];
            OutBufferSize = aOutBufferSize;
            Count = 0;
        }

        public void init()
        {
            // init serial port. using COM2 D2 & D3
            Serial = new SerialPort(SerialPorts.COM2, __baud__, Parity.None, 8, StopBits.One);
            Serial.Open();
        }

        public bool add_data(byte aByte)
        {
            if (Count == OutBufferSize)
                return false; // too big

            OutBuffer[Count++] = aByte;

            //Debug.Print("BYT " + aByte + " " + Count.ToString());
            return true;
        }

        public bool add_data(UInt16 aInt)
        {
            if (Count + 1 == OutBufferSize)
                return false; // too big

            OutBuffer[Count++] = (byte)(aInt >> 8); // MSB first
            OutBuffer[Count++] = (byte)(aInt);
            //Debug.Print("INT " + aInt + " " + (UInt16)((OutBuffer[Count - 2] << 8) + OutBuffer[Count - 1]) + " " + Count.ToString());
            //Debug.Print(" ---- component " + (OutBuffer[Count - 2]) + " " + (OutBuffer[Count - 1]));
            return true;
        }

        public void clear_buffer()
        {
            Array.Clear(OutBuffer, 0, Count);
            Count = 0;
        }

        public int send_data()
        {
            Debug.Print(" -------------------- send_data() " + Count);

            for (int i = 0; i < Count; i += __burst_size__)
            {
                if (i > 0)
                    Thread.Sleep(500);

                int n = __burst_size__;
                if (Count - i < n)
                    n = Count - i; // the difference

                Serial.Write(OutBuffer, i, n);
            }
            Thread.Sleep(400);

            int sent = Count;
            clear_buffer();            

            return sent;
        }
    }

}
