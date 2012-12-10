using System;
using Microsoft.SPOT;

namespace Helix
{
    class EvoController
    {
        public static GA GA = new GA();

        bool Play;
        int Step;

        //bool Selection;
        //bool Heredity;
        bool Mutation;
        bool MutationEvent;

        double current_mutation_rate;

        public EvoController()
        {
            Play = true;
            Step = 1;

            Mutation = true;
            MutationEvent = false;

            current_mutation_rate = GA.per_site_mutation_rate;
        }

        public void next()
        {
            if (Play)
            {
                if (MutationEvent)
                {
                    current_mutation_rate = GA.per_site_mutation_rate;
                    GA.per_site_mutation_rate = 0.5;
                }

                GA.evolve();

                if (MutationEvent)
                {
                    GA.per_site_mutation_rate = current_mutation_rate;
                    MutationEvent = false;
                }
            }
            else
            {
                if (Step > 0)
                {
                    GA.evolve();
                    Step--;
                }
            }
        }

        public void toggle_pauseplay()
        {
            Play = !Play;
        }

        public void mutation_event()
        {
            MutationEvent = true;
        }

        public void toggle_selection()
        {           
            GA.use_selection = !GA.use_selection;
        }

        public void toggle_mutation()
        {
            if (Mutation)
            {
                current_mutation_rate = GA.per_site_mutation_rate;
                GA.per_site_mutation_rate = 0;
                Mutation = false;
            }
            else
            {
                GA.per_site_mutation_rate = current_mutation_rate;
                Mutation = true;
            }
        }

        public void toggle_heredity()
        {
            GA.use_heredity = !GA.use_heredity;
        }


        public void take_step()
        {
            if (!Play) // this only makes sense in the context of being paused
                Step++;
        }

        // ceiling of 0.5
        public void raise_mut_rate()
        {
            if (Mutation && GA.per_site_mutation_rate < 0.5)
                GA.per_site_mutation_rate *= 2;
   
        }

        // floor of 0.01 
        public void lower_mut_rate()
        {
            if (Mutation && GA.per_site_mutation_rate > 0.01)
                GA.per_site_mutation_rate /= 2;
        }

        public void reset_ga() 
        {
            Play = true;
            Step = 1;
            GA.per_site_mutation_rate = 0.01;
            GA.use_heredity = true;
            GA.use_selection = true;
            GA.reset_population();
        }

        public int generation()
        {
            return GA.generation;
        }

        public double per_site_mutation_rate()
        {
            return GA.per_site_mutation_rate;
        }



    }
}
