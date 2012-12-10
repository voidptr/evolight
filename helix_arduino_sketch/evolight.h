#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

// Evolutionary Algorithm
#ifndef __POPULATION_SIZE__
    #define __POPULATION_SIZE__ 10 // seems to crashy crashy below this number :/
#endif

#ifndef __LOCI_COUNT__
    #define __LOCI_COUNT__ 12
#endif

#define __NUMBER_OF_TRAITS__ 3
#define __MAX_TRAIT_VARIATION__ 255
#define __GENOME_LENGTH__ __LOCI_COUNT__ * __NUMBER_OF_TRAITS__

class EvoLight {
public:
    struct Traits { byte r; byte g; byte b; };
private:
    float PerSiteMutationRate;

    Traits population[__POPULATION_SIZE__][__LOCI_COUNT__];
    Traits candidates[__POPULATION_SIZE__][__LOCI_COUNT__];
    long inverse_fitnesses[__POPULATION_SIZE__]; // index and fitness
    int generations;

public: 
    EvoLight() : generations(0), PerSiteMutationRate(0.01) 
    {
        // nothing happening here
//        generations = 0;
    }

    void init() 
    {
        //generations = 0;
        debug("EvoLight init()");
        Serial.println(generations);
        // random number seed
        randomSeed(analogRead(0));
        init_random_population();
        debug("END EvoLight init()");

    }
  
    void cycle_mutation_rate() 
    {
        PerSiteMutationRate = PerSiteMutationRate * 2;  
        if (PerSiteMutationRate > 0.5) { PerSiteMutationRate = 0.01; }
    }

    // reset the whole thing
    void reset_ga() 
    {
        PerSiteMutationRate = 0.01;
        init_random_population();
    }

    void evolve() 
    {
        debug("EvoLight evolve()");
        //init_random_population(); // debugging
        select_and_mutate();
        calculate_locus_differences__population();
        Serial.println(generations);
        generations = generations + 1;
        debug("END EvoLight evolve()");
    }

    Traits* get_trait(int org, int locus) 
    {
        return &population[org][locus];
    }

    int get_population_size() {
        return __POPULATION_SIZE__;
    }

    int get_loci_count() {
        return __LOCI_COUNT__;
    }

    long get_highest_fitness() 
    {
       return inverse_fitnesses[0];
    }

    int get_generations()
    {
        return generations;
    }

private:
    void select_and_mutate() 
    {
        select_candidates();
        mutate_candidates();
        recombine_candidates_into_population();
    }

    byte get_rand_trait() 
    {
        return byte(random(0, 256));
    }

    int is_mut() 
    {
        return random(0, 1000) < PerSiteMutationRate * 1000;
    }

    void copy_traits(Traits & source, Traits & target)
    {
        target.r = source.r;
        target.g = source.g;
        target.b = source.b;
    }

    void swap_traits(Traits & traits1, Traits & traits2)
    {
        Traits tmp;
        copy_traits(traits1, tmp);
        copy_traits(traits2, traits1);
        copy_traits(tmp, traits2);
    }

    void randomize_locus(int locus) 
    {
        for (int org = 0; org < __POPULATION_SIZE__; org++) 
        {  
            population[org][locus].r = get_rand_trait();
            population[org][locus].g = get_rand_trait();
            population[org][locus].b = get_rand_trait();
        }
    }

    void swap_genomes(int org_index1, int org_index2) 
    {
        Traits tmp;
        for (int locus = 0; locus < __LOCI_COUNT__; locus++) 
        {
            swap_traits( population[org_index1][locus], population[org_index2][locus] );
        }
    }

    void print_all_fitnesses() 
    {
        //Serial.println("all fit");
        for (int i = 0; i < __POPULATION_SIZE__; i++)
        {
            Serial.println(inverse_fitnesses[i]);
        }
    }
    void comb_sort(long input[], int n) 
    {  
        long swap;
        int i, gap = n;
        bool swapped = false;
 
        while ((gap > 1) || swapped) {
            if (gap > 1) {
                gap = (int)((double)gap / 1.247330950103979);
            }
 
            swapped = false;
 
            for (i = 0; gap + i < n; ++i) {                
                if (input[i] - input[i + gap] > 0) {
                    swap = input[i];
                    input[i] = input[i + gap];
                    input[i + gap] = swap;
                    swap_genomes(i, i + gap); // swap
                
                    swapped = true;
                }
            }
        }
    }

    // THIS METHOD WAS ANNOYING TO FIGURE OUT
    long diff_circular( byte trait1, byte trait2 ) 
    {
        long diff = 128 - abs( (abs(int(trait1)-int(trait2)) % 256) - 128);
        return diff;
    }

    long diff_linear( byte trait1, byte trait2 ) 
    {
        long diff = abs(int(trait1)-int(trait2));
        return diff;
    }

    long calculate_locus_differences( Traits genome[] )
    {
        long total_diff = 0, locus_diff = 0, trait_diff = 0;
        for (int locus = 0; locus < __LOCI_COUNT__ - 1; locus++) 
        { // each locus, allowing for sliding window
            locus_diff = abs( genome[locus].r - genome[locus + 1].r );
            locus_diff += abs( genome[locus].g - genome[locus + 1].g );
            locus_diff += abs( genome[locus].b - genome[locus + 1].b );

            total_diff += locus_diff;   
        }
   
        return total_diff;
    }

    long calculate_inverse_trait_variation( Traits genome[] )
    {
        long total_variation = 0;
        for (int locus = 0; locus < __LOCI_COUNT__; locus++) 
        { // each locus, allowing for sliding window
            int variation = 0;
            variation += diff_linear( genome[locus].r, genome[locus].g );
            variation += diff_linear( genome[locus].g, genome[locus].b );
            variation += diff_linear( genome[locus].b, genome[locus].r );

            total_variation += (__MAX_TRAIT_VARIATION__ - variation);
        }
   
        return total_variation;
    }

    void calculate_locus_differences__population() 
    {
        for (int i =0; i < __POPULATION_SIZE__; i++) 
        {
            long loc_diff = calculate_locus_differences( population[i] );
            long trait_var = calculate_inverse_trait_variation( population[i] );
      
            inverse_fitnesses[i] = loc_diff + trait_var;
        }
  
        //print_all_fitnesses();
        comb_sort(inverse_fitnesses, __POPULATION_SIZE__);
        //print_all_fitnesses();
        
    }

    void init_random_population() 
    {
        for (int org=0; org < __POPULATION_SIZE__; org++) 
        {  
            for (int locus=0; locus < __LOCI_COUNT__; locus++) 
            {    
                population[org][locus].r = get_rand_trait();
                population[org][locus].g = get_rand_trait();
                population[org][locus].b = get_rand_trait();
            } 
        }
 
        calculate_locus_differences__population();
    }


    // mutate a candidate genome. this is analogous to errors during gametogenensis. 
    void mutate(Traits genome[]) 
    { 
        for (int locus = 0; locus < __LOCI_COUNT__; locus++) 
        {
            if (is_mut()) { genome[locus].r = get_rand_trait(); }
            if (is_mut()) { genome[locus].g = get_rand_trait(); }
            if (is_mut()) { genome[locus].b = get_rand_trait(); }           
        }
    }

    // this overwrites whatever's in the target slot with whatever the source was
    void copy_genome(Traits source[], Traits target[]) 
    {
    //  Serial.println("copy_genome()");
        for (int locus = 0; locus < __LOCI_COUNT__; locus++) 
        {
            copy_traits(source[locus], target[locus]); 
        } 
    }

    void select_candidates() 
    {
        int l = 0, r = 0, random_index = 0, target = 0;
  
        // 50% of candidates are drawn from the top 25%
        l = 0;
        r = __POPULATION_SIZE__ * .25;
        for (int i = 0; i < (__POPULATION_SIZE__ * .5); i++) 
        {
            random_index = random(l, r + 1);
            copy_genome( population[random_index], candidates[target++] );
        }
    
        // 30% of candidates are drawn from the middle 50%
        l = __POPULATION_SIZE__ * .25;
        r = __POPULATION_SIZE__ * .75;  
        for (int i = 0; i < (__POPULATION_SIZE__ * .3); i++) {
            random_index = random(l, r + 1);
            copy_genome( population[random_index], candidates[target++] );
        }
  
        // 20% of candidates are drawn from the bottom 25%  
        l = __POPULATION_SIZE__ * .75;
        r = __POPULATION_SIZE__;
        for (int i = 0; i < (__POPULATION_SIZE__ * .2); i++) {
            random_index = random(l, r + 1);
            copy_genome( population[random_index], candidates[target++] );    
        }
    }

    void mutate_candidates() 
    {
        for (int i = 0; i < __POPULATION_SIZE__; i++) 
        {
            mutate(candidates[i]);
        }
    }

    void recombine_candidates_into_population() 
    {
        int target = 0;
        for (int i = 0; i < (__POPULATION_SIZE__ / 2); i++) 
        {
            int candidateX = random(0, __POPULATION_SIZE__);
            int candidateY = random(0, __POPULATION_SIZE__);
    
            copy_genome( candidates[candidateX], population[target++] );
            copy_genome( candidates[candidateY], population[target++] );
        
            int posA = random(0, __LOCI_COUNT__);
            int posB = random(0, __LOCI_COUNT__);
    
            bool recomb = false;
            for (int locus = 0; locus < __LOCI_COUNT__; locus++) 
            {
                if (locus == posA || locus == posB)
                {
                    recomb = !recomb;
                }
                if (recomb)
                {
                    swap_traits( population[target - 2][locus], population[target - 1][locus] );
                }
            }    
        }  
    }
};

/*
// selects from among the population (assumed to be sorted)
// mutates the population
// recombines among the population.
// there are some obvious problems here, especially with the the top organisms being 
// overrepresented in the final selectio process (because we are selecting in place,
// from bottom up), but this is probably the memory-cheapest way to go about it. :/
void select_and_mutate() {

  // SELECT
  int random_index = 0;
  int target_index = __POPULATION_SIZE__ - 1;
  int l, r, r_adj, samplesize; 
  // 10% of candidates from the bottom 20%
  l = (__POPULATION_SIZE__ * 0.2);
  r = __POPULATION_SIZE__;
  r_adj = r;
  samplesize = __POPULATION_SIZE__ * 0.1;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  // 40% of the candidates from the middle 60%
  l = (__POPULATION_SIZE__ * 0.2);
  r = (__POPULATION_SIZE__ * 0.8);
  r_adj = r;
  samplesize = __POPULATION_SIZE__ * 0.4;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  // 60% of candidates are from the top 20%
  l = 0;
  r = (__POPULATION_SIZE__ * 0.2);
  r_adj = r;
  samplesize = __POPULATION_SIZE__ * 0.4;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  
  // MUTATE
  for (int i = 0; i < __POPULATION_SIZE__; i++) {
    mutate(population[i]);
  }

  // RECOMBINE n/2 times.
  byte tmp;
  for (int i = 0; i < (__POPULATION_SIZE__ / 2); i++) {
    int candidateX = random(0, __POPULATION_SIZE__);
    int candidateY = random(0, __POPULATION_SIZE__);
    
    int posA = random(0, __GENOME_LENGTH__);
    int posB = random(0, __GENOME_LENGTH__);
    
    bool recomb = false;
    for (int j = 0; j < __GENOME_LENGTH__; j++) {
      if (j == posA || j == posB) {
        recomb = !recomb;
      }
      
      if (recomb) { // swap the values
        tmp = population[candidateX][j];
        population[candidateX][j] = population[candidateY][j];
        population[candidateY][j] = tmp;
      }
    }    
  }
} */




