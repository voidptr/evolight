using System;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;
using RC6_Remote;

namespace RC6_Remote
{
    public class Program
    {
        public static InterruptPort RC_In = new InterruptPort(Pins.GPIO_PIN_D7, false, Port.ResistorMode.Disabled, Port.InterruptMode.InterruptEdgeBoth);
        public static OutputPort outPort = new OutputPort(Pins.GPIO_PIN_D2, false);

        public static void Main()
        {
            // Declare our remote control input pin
            RC_In.OnInterrupt += new NativeEventHandler(RC_In_OnInterrupt);

            // Set the RC6 decoder's input pin to the one we just declared, and create an event handler for the code.
            RC6_Decoder.RemoteInputPin = RC_In;
            RC6_Decoder.CodeReceived += new CodeReceivedEventHandler(RC6_Decoder_CodeReceived);

            Thread.Sleep(Timeout.Infinite);
        }

        // Event handler for the RC6 pin's code event. Just pass everything to the handler in the RC6 decoder class
        static void RC_In_OnInterrupt(uint data1, uint data2, DateTime time)
        {
            RC6_Decoder.Record_Pulse(data1, data2, time);
        }

        // Event handler for the code received event
        static void RC6_Decoder_CodeReceived(int mode, ulong data)
        {
            Debug.Print("Received code! Mode=" + mode.ToString() + " ... Code=0x" + UlongToHexString(data));
        }



        // Hex to string. Codes make more sense in hex as they are really four bytes
        public static string UlongToHexString(ulong val)
        {
            string s = "";
            for (int i = 0; i < 8; i++)
            {
                ulong x = val & (ulong)0x0000000F;
                val = val >> 4;
                switch (x)
                {
                    case 0: s = '0' + s; break;
                    case 1: s = '1' + s; break;
                    case 2: s = '2' + s; break;
                    case 3: s = '3' + s; break;
                    case 4: s = '4' + s; break;
                    case 5: s = '5' + s; break;
                    case 6: s = '6' + s; break;
                    case 7: s = '7' + s; break;
                    case 8: s = '8' + s; break;
                    case 9: s = '9' + s; break;
                    case 10: s = 'A' + s; break;
                    case 11: s = 'B' + s; break;
                    case 12: s = 'C' + s; break;
                    case 13: s = 'D' + s; break;
                    case 14: s = 'E' + s; break;
                    case 15: s = 'F' + s; break;
                }
            }
            return s;
        }


    }
}
