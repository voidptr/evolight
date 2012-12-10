using System;

namespace Helix
{
    class GA
    {
        private const int __population_size__ = 20;
        public const int __loci_count__ = 30;
        private const int __max_colorfulness__ = __loci_count__ * 255 * 2;

        private static double per_site_mutation_rate = 0.01;
        private int generation;

        protected static Random random_generator = new Random();

        public Organism[] Population = new Organism[__population_size__];
        private Organism[] Candidates = new Organism[__population_size__];

        public GA()
        {
            random_generator = new Random();
            generation = 0;
            init();
        }

        public void init()
        {
            init_random_population();
        }

        public static bool is_mut()
        {
            if (per_site_mutation_rate * 1000 < random_generator.Next(999))
                return true;
            return false;
        }

        public void evolve()
        {
            select_candidates();
            mutate_candidates();
            recombine_candidates_into_population();
            sort_population();

            generation++;
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

        private void mutate_candidates()
        {
            foreach (Organism aOrg in Candidates)
                aOrg.mutate();
        }

        private void select_candidates()
        {
//            select_candidates__elitism();
            select_candidates__hybrid();
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

        public void reset_ga()
        {
            per_site_mutation_rate = 0.01;
            init_random_population();
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
                return (byte)(GA.random_generator.Next(255));
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
                    b = get_random();
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
                inverse_fitness = sumDiff + invSumColor;
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
                foreach (Trait aTrait in Traits)
                {
                    aTrait.mutate();
                }
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
    }
}
