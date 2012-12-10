using System.Threading;
using Microsoft.SPOT;
using System;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;
using NecDecoder;

namespace Helix
{
    public class Program
    {
        const int __input_pads__ = 3;
        const int __status_lights__ = 10;
        const int __evo_lights__ = 290;
        const int __num_lights__ = __status_lights__ + __evo_lights__;
        const int __num_organism_to_represent__ = 10;

        // the various classes that do the various things
        static EvoController Evo = new EvoController();
        static Serial_Communications_Async Comms = 
            new Serial_Communications_Async(__num_lights__ * 5);
        static RemoteController Remote = new RemoteController();
        
        static byte[] comparison_buffer = new byte[__num_lights__ * 3]; // the bare bytes


        static double brightness = 1.0;
        static bool send_all;


        public static void Main()
        {            
            Thread.Sleep(1000); // IN CASE I FUCKED SOMETHING UP AND I NEED TO CATCH IT

            Comms.init();
            while (true)
            {
                //Debug.Print(Debug.GC(false).ToString());
                remote_actions();
                evolution_actions();
                communications_actions();
            }
        }

        private static void remote_actions()
        {
            // EvoController responsibilities
            if (Remote.call_ga_reset)
            {
                Remote.call_ga_reset = false;
                Evo.reset_ga();
            }
            if (Remote.call_pauseplay)
            {
                Remote.call_pauseplay = false;
                Evo.toggle_pauseplay();
            }
            if (Remote.call_evostep)
            {
                Remote.call_evostep = false;
                Evo.take_step();
            }
            if (Remote.call_raisemutrate)
            {
                Remote.call_raisemutrate = false;
                Evo.raise_mut_rate();
            }
            if (Remote.call_lowermutrate)
            {
                Remote.call_lowermutrate = false;
                Evo.lower_mut_rate();
            }
            if (Remote.call_mutationevent)
            {
                Remote.call_mutationevent = false;
                Evo.mutation_event();
            }
            if (Remote.call_toggleheredity)
            {
                Remote.call_toggleheredity = false;
                Evo.toggle_heredity();
            }
            if (Remote.call_togglemutation)
            {
                Remote.call_togglemutation = false;
                Evo.toggle_mutation();
            } 
            if (Remote.call_toggleselection)
            {
                Remote.call_toggleselection = false;
                Evo.toggle_selection();
            }
            // my responsibility
            if (Remote.call_raisebrightness)
            {
                Remote.call_raisebrightness = false;
                raise_brightness();
            }
            if (Remote.call_lowerbrightness)
            {
                Remote.call_lowerbrightness = false;
                lower_brightness();
            }

        }

        private static void lower_brightness()
        {
            if (brightness > 0.11)
                brightness -= 0.1;
            Debug.Print(brightness.ToString());
            send_all = true;
        }

        private static void raise_brightness()
        {
            if (brightness < 0.99)
                brightness += 0.1;
            Debug.Print(brightness.ToString());
            send_all = true;
        }

        private static void evolution_actions()
        {           
            Evo.next(); // considering the state of the evolution engine, do the right thing.
        }

        private static void communications_actions()
        {
            prepare_output_buffer();
            Comms.send_data();
        }
        
        private static void prepare_output_buffer()
        {
            if (Evo.generation() % 30 == 0 || send_all)
            {
                send_all = false;
                prepare_output__all();
            }
            else
                prepare_output__differences();

            prepare_status_lights();
        }

        private static void prepare_status_lights()
        {
            // status light - blinks as generations advance
            byte val = (byte)((Evo.generation() % 2) * 254); // 0 or 254 
            Comms.add_data((UInt16)290);
            Comms.add_data(adjust_for_brightness(val));
            Comms.add_data(adjust_for_brightness(val));
            Comms.add_data(adjust_for_brightness(val));

            // mutation rate light - goes from green to red as rate gets tougher
            byte red = (byte)( (Evo.per_site_mutation_rate() * 3) * 254 );
            byte green = (byte)(254 - ((Evo.per_site_mutation_rate() * 3) * 254));
            Comms.add_data((UInt16)291);
            Comms.add_data(adjust_for_brightness(red)); // red from 0 - 254
            Comms.add_data(adjust_for_brightness(green)); // green from 254 - 0
            Comms.add_data(adjust_for_brightness(0));

            // selection status light
            val = (byte)((GA.use_selection ? 1 : 0) * 254); // 0 or 254 
            Comms.add_data((UInt16)292);
            Comms.add_data(adjust_for_brightness(val)); // red from 0 - 254
            Comms.add_data(adjust_for_brightness(val)); // green from 254 - 0
            Comms.add_data(adjust_for_brightness(val));

            // heredity status light
            val = (byte)((int)(GA.use_heredity ? 1 : 0) * 254); // 0 or 254 
            Comms.add_data((UInt16)293);
            Comms.add_data(adjust_for_brightness(val)); // red from 0 - 254
            Comms.add_data(adjust_for_brightness(val)); // green from 254 - 0
            Comms.add_data(adjust_for_brightness(val));

            // mutation status light
            val = (byte)((int)((Evo.per_site_mutation_rate() > 0) ? 1 : 0) * 254); // 0 or 254 
            Comms.add_data((UInt16)294);
            Comms.add_data(adjust_for_brightness(val)); // red from 0 - 254
            Comms.add_data(adjust_for_brightness(val)); // green from 254 - 0
            Comms.add_data(adjust_for_brightness(val));

            for (int i = 295; i < 300; i++)
            {
                Comms.add_data((UInt16)i);
                Comms.add_data(adjust_for_brightness(128));
                Comms.add_data(adjust_for_brightness(128));
                Comms.add_data(adjust_for_brightness(128));
            }
        }

        private static byte adjust_for_brightness(byte aVal)
        {
            return (byte)(aVal * brightness);
        }

        private static void prepare_output__differences()
        {
            UInt16 lPixelNum = 0;
            int lComparisonPos = 0;

            int[] seen = new int[__num_organism_to_represent__];

            for (int i = 0; i < __num_organism_to_represent__; i++)
            {
                // randomly pull without replacement from the population to see which ones we show
                int index = 0;
                while (true)
                {
                    index = GA.random_generator.Next(GA.__population_size__);
                    if (Array.IndexOf(seen, index, 0, i) < 0) // I aint seened it 
                    {
                        seen[i] = index;
                        break;
                    }
                }

                GA.Organism org = EvoController.GA.Population[index];
                foreach (GA.Trait trait in org.Traits)
                {
                    if ((comparison_buffer[lComparisonPos] != trait.r ||
                        comparison_buffer[lComparisonPos + 1] != trait.g ||
                        comparison_buffer[lComparisonPos + 2] != trait.b) ) // mismatch!
                    {
                        // populate the comms buffer.
                        Comms.add_data(lPixelNum);
                        Comms.add_data(adjust_for_brightness(trait.r));
                        Comms.add_data(adjust_for_brightness(trait.g));
                        Comms.add_data(adjust_for_brightness(trait.b));

                        // update the comparison buffer
                        comparison_buffer[lComparisonPos] = trait.r;
                        comparison_buffer[lComparisonPos + 1] = trait.g;
                        comparison_buffer[lComparisonPos + 2] = trait.b;
                    }

                    lPixelNum++; // one pixel
                    lComparisonPos += 3; // there are only three bytes of info in the comparison array
                }
            }
        }

        private static void prepare_output__all()
        {
            UInt16 lPixelNum = 0;
            int lComparisonPos = 0;

            int[] seen = new int[__num_organism_to_represent__];

            for (int i = 0; i < __num_organism_to_represent__; i++)
            {
                // randomly pull without replacement from the population to see which ones we show
                int index = 0;
                while (true)
                {
                    index = GA.random_generator.Next(GA.__population_size__);
                    if (Array.IndexOf(seen, index, 0, i) < 0) // I aint seened it 
                    {
                        seen[i] = index;
                        break;
                    }
                }
                GA.Organism org = EvoController.GA.Population[index];
                foreach (GA.Trait trait in org.Traits)
                {
                    // populate the comms buffer.
                    Comms.add_data(lPixelNum);
                    Comms.add_data(adjust_for_brightness(trait.r));
                    Comms.add_data(adjust_for_brightness(trait.g));
                    Comms.add_data(adjust_for_brightness(trait.b));

                    // update the comparison buffer
                    comparison_buffer[lComparisonPos] = trait.r;
                    comparison_buffer[lComparisonPos + 1] = trait.g;
                    comparison_buffer[lComparisonPos + 2] = trait.b;                   

                    lPixelNum++; // one pixel
                    lComparisonPos += 3; // there are only three bytes of info in the comparison array
                }
            }
        }

        private static void populate_lights__debug()
        {
            for (int i = 0; i < 300; i++)
            {
                UInt32 c = Wheel( (byte)i);
                Comms.add_data((UInt16)i);
                Comms.add_data(ToR(c));
                Comms.add_data(ToG(c));
                Comms.add_data(ToB(c));
            }
        }

        private static byte ToR(UInt32 aColor)
        {
            return (byte)(aColor >> 16);
        }

        private static byte ToG(UInt32 aColor)
        {
            return (byte)(aColor >> 8);
        }

        private static byte ToB(UInt32 aColor)
        {
            return (byte)(aColor);
        }


        private static UInt32 Wheel(byte WheelPos)
        {
            byte A = (byte)(WheelPos * 3);
            byte B = (byte)(255 - WheelPos * 3);
            byte C = 0;

            if (WheelPos < 85)
                return Color(A, B, C);
            else if (WheelPos < 170)
                return Color(B, C, A);
            else
                return Color(C, A, B);
        }

        private static UInt32 Color(byte r, byte g, byte b)
        {
            UInt32 c;
            c = r;
            c <<= 8;
            c |= g;
            c <<= 8;
            c |= b;
            return c;
        }


    }
}
