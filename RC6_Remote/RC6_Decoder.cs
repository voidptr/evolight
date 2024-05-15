using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using System.Threading;

namespace RC6_Remote
{
    // event handler delegate
    public delegate void CodeReceivedEventHandler(int mode, ulong data);

    /// <summary>
    /// RC6 remote decoder. Tested only on RC6 modes 0 and 6 (microsoft special 4 byte mode). Fully static, no need to
    /// instantiate the class. NOT optimised - intentionall to make it easier to read and understand.
    /// </summary>
    class RC6_Decoder
    {

        /// <summary>
        /// This is the RC pin. It MUST be initialised to a valid InterruptPort in your main program or you will get an
        /// invalid object reference error at runtime. Nice C# code has accessors, of course.
        /// </summary>
        public static InterruptPort RemoteInputPin;

        public static event CodeReceivedEventHandler CodeReceived;

        /// <summary>
        /// Here are our attribute declarations. 
        /// </summary>
        private const int rc_timeout = 5;    // RC reception times out after 5 milliseconds
        private static int pos = 0;
        private static long[] intervals = new long[256];
        private static Boolean[] signalStates = new Boolean[256];
        private static Timer RCtimeoutTimer = new Timer(new TimerCallback(RCtimeout), null, Timeout.Infinite, Timeout.Infinite);

        /// <summary>
        /// This is the pulse recorder. It should be called from the interrupt handler routine of your main program
        /// </summary>
        /// <param name="data1"></param>
        /// <param name="data2"></param>
        /// <param name="time"></param>
        public static void Record_Pulse(uint data1, uint data2, DateTime time)
        {
            intervals[pos] = time.Ticks / 10;       // record the interrupt time in microseconds
            signalStates[pos] = (data2 == 1);       // record the state of the pin at interrupt
            pos++;
            if (pos > intervals.Length) pos = 0;
            RCtimeoutTimer.Change(rc_timeout, Timeout.Infinite);    // set / reset the timeout timer
        }

        /// <summary>
        /// This is a timer responder. Each time a bit is received, the timer is reset. If no bits are received after a
        /// period defined by the rc_timeout constant in this class then the quiet time is deemed to have elapsed
        /// and the received pulse train will be decoded.
        /// </summary>
        /// <param name="o"></param>
        static void RCtimeout(object o)
        {
            // Disable the RC interrupt, process the command, clear the buffers then re-enable the interrupt
            intervals[pos] = DateTime.Now.Ticks / 10;
            signalStates[pos] = RemoteInputPin.Read();
            RCtimeoutTimer.Change(Timeout.Infinite, Timeout.Infinite);  // Stop the timer
            RC6_Decoder.Decode(intervals, signalStates);
            pos = 0;
            intervals = new long[256];              // The garbage collector is going to need to run for this at some point!
            signalStates = new Boolean[256];        // But when? Probably in the middle of an interrupt. This bad code! :^)
        }

        /// <summary>
        /// This is the decoder. It uses a state machine to work through two arrays, one of
        /// bit timings and one of bit states, to get the RC6 code string. 
        /// </summary>
        /// <param name="timingArray"> // array of bit timings in microseconds</param>
        /// <param name="stateArray"> // array of bit states</param>
        /// <returns></returns>
        /// 
        public static void Decode(long[] timingArray, Boolean[] stateArray)
        {
            // Process into a sequence of time values - normalise to proper multiples of 36kHz half-bit-time pulses. Note
            // that there is quite a bit of tolerance in the bit timings - this is to allow for the .NET MF's lack of
            // a deterministic interrupt - it's not a Real Time OS! You will get the odd bad read if something happens to block
            // the interrupts while reading a sequence, but it's a remote control .. they have bad reads all the time anyway!
            int totalhalfBits = 0;
            int[] pulseArray = new int[timingArray.Length];
            for (int j = 1; j < timingArray.Length; j++)
            {
                int pulse = (int)(timingArray[j] - timingArray[j - 1]);
                if (pulse >= 300 && pulse <= 600)
                {
                    // A single half bit length
                    if (stateArray[j] == true) pulseArray[j] = 1;
                    else pulseArray[j] = -1;
                }
                else if (pulse >= 680 && pulse <= 1080)
                {
                    // two half bit lengths
                    if (stateArray[j] == true) pulseArray[j] = 2;
                    else pulseArray[j] = -2;
                }
                else if (pulse >= 1150 && pulse <= 1450)
                {
                    // three half bit lengths
                    if (stateArray[j] == true) pulseArray[j] = 3;
                    else pulseArray[j] = -3;
                }
                else if (pulse >= 2400 && pulse <= 2800)
                {
                    // six half bit lengths
                    if (stateArray[j] == true) pulseArray[j] = 6;
                    else pulseArray[j] = -6;
                }
                else if (pulse > 3000)
                {
                    // a long quiet time has been recorded - end of signal                    
                    if ((totalhalfBits % 2) > 1) // must be even, if not there is a 0 half bit in the quiet time
                    {
                        pulseArray[j] = -1;     // Still represents a 12 half bit.
                        pulseArray[j + 1] = 0;  // Identify the end of the pulse train.
                        totalhalfBits++;        // Add this last half bit as the goto will miss that
                    }
                    goto ExitLoop;          // break out of the loop
                }
                else
                {
                    //Debug.Print("error converting " + pulse.ToString() + "us pulse at " + j.ToString());
                }

                totalhalfBits += System.Math.Abs(pulseArray[j]);     // Accumulate the total number of half-bits
            }

        ExitLoop:       // Break out point of the decoding loop above

            // Now go through pulses and decode them into a stream of half-bit states (ie TFFTFT = 100 in Manchester encoding)
            int k = 0;
            Boolean[] stateStream = new Boolean[128];
            for (int j = 1; j < pulseArray.Length; j++)
            {
                for (int i = 0; i < System.Math.Abs(pulseArray[j]); i++)
                    stateStream[k++] = stateArray[j];
                // Look for a zero length indicating the end of the bit stream. Ignore the initial leader which may be zero now
                if (j > 2 && pulseArray[j] == 0)
                    break;
            }

            // Finally, decode the resulting final messages per RC6. 
            //

            // First test for the leader - 6 high half-bits and two low half-bits
            if (!(stateStream[0] == true && stateStream[1] == true && stateStream[2] == true && stateStream[3] == true
                && stateStream[4] == true && stateStream[5] == true && stateStream[6] == false && stateStream[7] == false))
            {
                //Debug.Print("Did not find the correct RC6 Leader.");
            }

            // We're looking now for the start bit which should always be a "1"
            if (!(stateStream[8] == true && stateStream[9] == false))
            {
                // Bad start bit - bomb out
                //Debug.Print("Error reading start bit");
            }

            // Now decode the mode info
            int mode = 0x00;
            for (int i = 10; i < 15; i += 2)
            {
                if (stateStream[i] == true && stateStream[i + 1] == false)
                    mode = (mode << 1) + 0x01;
                else if (stateStream[i] == false && stateStream[i + 1] == true)
                    mode = (mode << 1) + 0x00;
                else
                {
                    // Bad mode read - bomb out
                    //Debug.Print("Error reading mode bits");
                }
            }

            // Look for the mode trailer - trailer bits are double length. Can be a 1 or 0 depending on the mode
            int trailer = 0x00;
            if (stateStream[16] == false && stateStream[17] == false && stateStream[18] == true && stateStream[19] == true)
                trailer = 0x00;
            else if (stateStream[16] == false && stateStream[17] == false && stateStream[18] == true && stateStream[19] == true)
                trailer = 0x01;
            else
            {
                // Bad trailer bit - bomb out
                //Debug.Print("Error reading trailer bit");
            }

            // Now get the code. can't be more than a long (4 bytes). RC6 will be two bytes, but the Microsoft remotes seem
            // to use a 4 byte code as an RC6 derivative. Only modes 0 and 6 (microsoft) tested!
            ulong data = 0x00;
            for (int i = 20; i < totalhalfBits; i += 2)
            {
                if (stateStream[i] == true && stateStream[i + 1] == false)
                    data = (data << 1) + 0x01;
                else if (stateStream[i] == false && stateStream[i + 1] == true)
                    data = (data << 1) + 0x00;
                else
                {
                    // Bad mode read - bomb out
                    //Debug.Print("Error reading command / data at half bit # " + (i + 1).ToString());
                }
            }

            // Let's see what we got!
            //Debug.Print("Mode = " + mode.ToString() + " trailer = " + trailer.ToString() + "Command / data = 0x" + data.ToString()
            //    + " = " + UlongToHexString(data));

            if (CodeReceived != null)
                CodeReceived(mode, data);
        }

    }
}
 
