using System;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;
using Microsoft.SPOT;

namespace Helix
{
    class GA
    {
        private const bool __debug__ = true;

        public const int __population_size__ = 20;
        public const int __loci_count__ = 29;
        private const byte __max_trait_value__ = 254;
        private const int __max_colorfulness__ = __loci_count__ * __max_trait_value__ * 2;
       

        public static double per_site_mutation_rate = 0.01;
        public int generation;

        public static bool use_heredity = true;
        public static bool use_selection = true;

        //public int fitness;

        public static Random random_generator;

        public Organism[] Population = new Organism[__population_size__];
        private Organism[] Candidates = new Organism[__population_size__];

        public GA()
        {
            AnalogInput randomseedpin = new AnalogInput(AnalogChannels.ANALOG_PIN_A0);
            random_generator = new Random(randomseedpin.ReadRaw() * randomseedpin.ReadRaw());
            randomseedpin.Dispose();

            generation = 0;
            init();
        }

        public void init()
        {
            init_random_population();
        }

        public static bool is_mut()
        {
            int randval = random_generator.Next(999);
            if (per_site_mutation_rate * 1000 > randval)
                return true;
            return false;
        }

        public void evolve()
        {
            select_candidates();
            
            mutate_candidates();

            reproduce_candidates();
            sort_population();
            //print_fitnesses();

            generation++;
        }

        // use selection (or not)
        private void select_candidates()
        {
//            select_candidates__elitism();
            if (use_selection)
                select_candidates__hybrid();
            else
                select_candidates__random();
        }

        private void mutate_candidates()
        {
            foreach (Organism aOrg in Candidates)
                aOrg.mutate();
        }

        // use heredity (or not)
        private void reproduce_candidates()
        {
            if (use_heredity)
                recombine_candidates_into_population();
            else
                init_random_population();
        }

        private void print_fitnesses()
        {
            foreach (Organism lOrg in Population)
            {
                print_debug(lOrg.inverse_fitness.ToString());
            }
        }

        private void sort_population()
        {
            comb_sort(Population, __population_size__);
        }

        void comb_sort(Organism[] input, int n) 
        {  
            Organism swap;
            int i, gap = n;
            bool swapped = false;
 
            while ((gap > 1) || swapped) {
                if (gap > 1) {
                    gap = (int)((double)gap / 1.247330950103979);
                }
 
                swapped = false;
 
                for (i = 0; gap + i < n; ++i) {                
                    if (input[i].inverse_fitness - input[i + gap].inverse_fitness > 0) {
                        swap = input[i];
                        input[i] = input[i + gap];
                        input[i + gap] = swap;
                    
                        //swap_genomes(i, i + gap); // swap
                
                        swapped = true;
                    }
                }
            }
        }



        private void recombine_candidates_into_population()
        {
            for (int i = 0; i < __population_size__; i += 2)
            {
                Population[i] = new Organism( Candidates[ random_generator.Next(__population_size__ - 1) ] );
                Population[i+1] = new Organism( Candidates[ random_generator.Next(__population_size__ - 1) ] );

                Organism.recombine(Population[i], Population[i + 1]);
            }
        }

        // select randomly from the population
        private void select_candidates__random()
        {
            for (int i = 0; i < __population_size__; i++)
            {
                Candidates[i] = new Organism(
                    Population[random_generator.Next((int)(__population_size__))]);
            }
        }

        // select all candidates from the top 50%
        private void select_candidates__elitism()
        {
            for (int i = 0; i < __population_size__; i++)
            {
                Candidates[i] = new Organism(
                    Population[random_generator.Next((int)(__population_size__ * .5))]);
            }
        }

        private void select_candidates__hybrid()
        {
            // 50% are drawn from top 20%
            int target = 0;
            for (int i = 0; i < (int)(__population_size__ * .5); i++, target++)
            {
                int popCandidate = random_generator.Next((int)(__population_size__ * .2) - 1);
                Candidates[target] = new Organism(
                    Population[popCandidate]);
            }
            // 30% are drawn from middle 60%
            for (int i = 0; i < (int)(__population_size__ * .3); i++, target++)
            {
                Candidates[target] = new Organism(
                    Population[random_generator.Next((int)(__population_size__ * .6)) + 
                    (int)(__population_size__ * .2) - 1]);
            }
            // 20% are drawn from bottom 20%
            for (int i = 0; i < (int)(__population_size__ * .2); i++, target++)
            {
                Candidates[target] = new Organism(
                    Population[random_generator.Next((int)(__population_size__ * .2)) +
                    (int)(__population_size__ * .8) - 1]);
            }
        }

        public void reset_population()
        {
            init_random_population();
            generation = 0;
        }

        private void init_random_population()
        {
            for (int i = 0; i < __population_size__; i++)
            {
                Population[i] = new Organism();
                Population[i].randomize();
            }

        }

        public struct Trait
        {
            public byte r, g, b;

            public Trait(byte rVal, byte gVal, byte bVal)
            {
                r = rVal;
                g = gVal;
                b = bVal;
            }

            public void randomize()
            {
                r = get_random();
                g = get_random();
                b = get_random();
            }

            private byte get_random()
            {
                return (byte)(GA.random_generator.Next(__max_trait_value__));
            }

            public int calculate_colorfulness()
            {
                return System.Math.Abs(r - g) + System.Math.Abs(g - b) + System.Math.Abs(b - r);
            }

            public static int calculate_difference(Trait aTraitA, Trait aTraitB)
            {
                return System.Math.Abs(aTraitA.r - aTraitB.r) +
                    System.Math.Abs(aTraitA.g - aTraitB.g) +
                    System.Math.Abs(aTraitA.b - aTraitB.b);
            }

            public void mutate()
            {
                if (is_mut())
                    r = get_random();
                if (is_mut())
                    g = get_random();
                if (is_mut())
                    b = get_random();
            }
        }

        public class Organism : IComparable
        {
            public Trait[] Traits = new Trait[__loci_count__];
            public int inverse_fitness;

            public Organism()
            {
                inverse_fitness = int.MaxValue;
            }

            public Organism( Organism aOriginal ) 
            {
                for (int i = 0; i < __loci_count__; i++)
                    Traits[i] = aOriginal.Traits[i];
                //Array.Copy(Traits, aOriginal.Traits, __loci_count__);
                inverse_fitness = aOriginal.inverse_fitness;
            }

            public void randomize()
            {
                for (int i = 0; i < __loci_count__; i++)
                {
                    Traits[i].randomize();
                }
                calculate_fitness();
            }

            public void calculate_fitness()
            {
                int sumDiff = 0;
                for (int i = 0; i < __loci_count__ - 1; i++)
                {
                    sumDiff += Trait.calculate_difference(Traits[i], Traits[i+1]);
                }
                sumDiff += Trait.calculate_difference(Traits[0], Traits[__loci_count__ - 1]);

                int sumColor = 0;
                foreach (Trait aTrait in Traits)
                {
                    sumColor += aTrait.calculate_colorfulness();
                }
                int invSumColor = __max_colorfulness__ - sumColor;
                inverse_fitness = (sumDiff * 4) + invSumColor;
            }

            int IComparable.CompareTo(object obj)
            {
                if (obj is Organism) {
                    Organism aOrg = (Organism) obj;
                    if (inverse_fitness < aOrg.inverse_fitness)
                        return -1;
                    if (inverse_fitness > aOrg.inverse_fitness)
                        return 1;
                    return 0;
                }
                throw new ArgumentException("object is not an Organism");    
            }

            internal void mutate()
            {
                for (int i = 0; i < Traits.Length; i++)
                {
                    Traits[i].mutate();
                }
//                foreach (Trait aTrait in Traits)
//                {
//                    aTrait.mutate();
//                }
            }
            internal void mutate(int index)
            {
                Traits[index].mutate();
            }

            internal static void recombine(Organism organism, Organism organism_2)
            {
                int posA = random_generator.Next(__loci_count__ - 1);
                int posB = random_generator.Next(__loci_count__ - 1);

                bool recomb = false;
                for (int i = 0; i < __loci_count__; i++)
                {
                    if (i == posA || i == posB)
                        recomb = !recomb;

                    if (recomb) // swap
                    {
                        Trait lTmp = organism.Traits[i];
                        organism.Traits[i] = organism_2.Traits[i];
                        organism_2.Traits[i] = lTmp;
                    }
                }

                organism.calculate_fitness();
                organism_2.calculate_fitness();
            }
        }

        private static void print_debug(string aMessage)
        {
            if (__debug__)
            {
                Microsoft.SPOT.Debug.Print(aMessage);
            }
        }
    }
}
