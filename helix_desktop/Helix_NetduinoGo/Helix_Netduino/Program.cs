using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;


namespace Helix
{
    public class Program
    {
        const int __input_pads__ = 3;
        const int __num_lights__ = 60;

        static GA Evo = new GA();
        static Serial_Communications Comms = new Serial_Communications(__input_pads__, __num_lights__);

        public static void Main()
        {
            Debug.EnableGCMessages(true);
            Comms.init();

            while (true)
            {
                Evo.evolve();
                for (int i = 0; i < GA.__loci_count__; i++)
                {
                    Thread.Sleep(10);
                    Debug.Print("Memory: " + Microsoft.SPOT.Debug.GC(false).ToString());
                }
                Debug.Print("Starting comms. populate_lights()");
                populate_lights();
                Debug.Print("Doing comms. Comms.communicate()");
                Comms.communicate();
                Debug.Print("Reading sensors. read_sensors()");
                read_sensors();

            }

        }

        private static void read_sensors()
        {
            Debug.Print(Comms.InBuffer.ToString()); // whatever. Will fix.
        }

        private static void populate_lights()
        {
            int lCount = 0;
            foreach( GA.Organism org in Evo.Population )
                foreach (GA.Trait trait in org.Traits)
                {
                    Comms.OutBuffer[lCount++] = trait.r;
                    Comms.OutBuffer[lCount++] = trait.g;
                    Comms.OutBuffer[lCount++] = trait.b;

                    if (lCount > Comms.OutBuffer.Length) // enough!
                        return;
                }
        }

    
    }
}
