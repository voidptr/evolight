#include "utils.h"

/// evolution program setup
const byte __length__ = 9; // the number of loci
const byte __traits__ = 3; // the number of traits per loci (probably wrong phrasing)
const byte __population_size__ = 24;
float __per_site_mutation_rate__ = 0.01;
const byte __max_trait_val__ = 255;
const int __genome_length__ = __length__ * __traits__;
const float __selection_threshold__ = 0.25;
const long __max_diff_val__ = __max_trait_val__ * __genome_length__;
const long __max_trait_variation__ = __traits__ * 255;

byte population[__population_size__][__length__][__traits__]; // population
byte candidates[__population_size__][__length__][__traits__]; // population
long inverse_fitnesses[__population_size__]; // index and fitness


void cycle_mutation_rate() {
  __per_site_mutation_rate__ = __per_site_mutation_rate__ * 2;
  
  if (__per_site_mutation_rate__ > 0.5) { __per_site_mutation_rate__ = 0.01; }
  
  Serial.print("Mutation Rate: ");
  Serial.println(__per_site_mutation_rate__); 
}

byte get_rand_trait() {
  return byte(random(0, 256));
}

int is_mut() {
  return random(0, 1000) < __per_site_mutation_rate__ * 1000;
}


void randomize_locus(int locus) {
  for (int org = 0; org < __population_size__; org++) {  
    for (int trait = 0; trait < __traits__; trait++) {
      population[org][locus][trait] = get_rand_trait();
    }
  }
}

void swap_genomes(int org_index1, int org_index2) {

  byte tmp = 0;
  for (int locus = 0; locus < __length__; locus++) {
    for (int trait = 0; trait < __traits__; trait++) {
      tmp = population[org_index1][locus][trait];
      population[org_index1][locus][trait] = population[org_index2][locus][trait];
      population[org_index2][locus][trait] = tmp;
    }
  }
}

void comb_sort(long input[], int n) {
    
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
long diff_circular( byte trait1, byte trait2 ) {
  long diff = 128 - abs( (abs(int(trait1)-int(trait2)) % 256) - 128);
  return diff;
}

long diff_linear( byte trait1, byte trait2 ) {
  long diff = abs(int(trait1)-int(trait2));
  return diff;
}

long calculate_locus_differences( byte genome[][__traits__] )
{
  long total_diff = 0, locus_diff = 0, trait_diff = 0;
  for (int locus = 0; locus < __length__ - 1; locus++) { // each locus, allowing for sliding window
    locus_diff = 0;
    for (int trait = 0; trait < __traits__; trait++) {
      trait_diff = abs( genome[locus][trait] - genome[locus + 1][trait] );
      locus_diff += trait_diff;
    }
    total_diff += locus_diff;   
  }
   
  return total_diff;
}

long calculate_inverse_trait_variation( byte genome[][__traits__] )
{
  long total_variation = 0;
  for (int locus = 0; locus < __length__; locus++) { // each locus, allowing for sliding window
    int variation = 0;
    for (int trait = 0; trait < __traits__ - 1; trait++) {
      variation += diff_linear( genome[locus][trait], genome[locus][trait + 1] );
    }
    variation += diff_linear( genome[locus][__traits__ - 1], genome[locus][0] );
    total_variation += (__max_trait_variation__ - variation);
  }
   
  return total_variation;
}

void calculate_locus_differences__population() {
  for (int i =0; i < __population_size__; i++) {

    long loc_diff = calculate_locus_differences( population[i] );
    long trait_var = calculate_inverse_trait_variation( population[i] );
    
    Serial.print(loc_diff);
    Serial.print(" - ");    
    Serial.println(trait_var);
    
    inverse_fitnesses[i] = loc_diff + trait_var;
  }
  
  comb_sort(inverse_fitnesses, __population_size__);
}

void print_best_inverse_fitness() {
  Serial.println(inverse_fitnesses[0]);  
}

void print_population_inverse_fitnesses() {
  for (int i = 0; i < __population_size__; i++) { 
    Serial.println(inverse_fitnesses[i]);  
  }
}

void init_random_population() {
 
  for (int org=0; org < __population_size__; org++) {  
    for (int locus=0; locus < __length__; locus++) {     
      for (int trait=0; trait < __traits__; trait++) {
        population[org][locus][trait] = get_rand_trait();
      }
    } 
  }
 
  calculate_locus_differences__population();
}

// reset the whole thing
void reset_ga() {
  __per_site_mutation_rate__ = 0.01;
  init_random_population();
}

// mutate a candidate genome. this is analogous to errors during gametogenensis. 
void mutate(byte genome[][__traits__]) { 
  for (int locus = 0; locus < __length__; locus++) {
    for (int trait = 0; trait < __traits__; trait++) {
      if (is_mut()) { // mutate
        genome[locus][trait] = get_rand_trait();
      }
    }
  }
}

// this overwrites whatever's in the target slot with whatever the source was
void copy_genome(byte source[][__traits__], byte target[][__traits__]) {
//  Serial.println("copy_genome()");
  for (int locus = 0; locus < __length__; locus++) {
    for (int trait = 0; trait < __traits__; trait++) {
      target[locus][trait] = source[locus][trait];
    }
  } 
}

void select_candidates() {
  int l = 0, r = 0, random_index = 0, target = 0;
  
  // 50% of candidates are drawn from the top 25%
  l = 0;
  r = __population_size__ * .25;
  Serial.print("Top 25% -> 50% ");
  Serial.print(l);
  Serial.print(" ");
  Serial.println(r);  
  for (int i = 0; i < (__population_size__ * .5); i++) {
    random_index = random(l, r + 1);
    Serial.print(random_index);
    Serial.print(" --> ");
    Serial.println(target); 
    copy_genome( population[random_index], candidates[target++] );
  }
  
  
  // 30% of candidates are drawn from the middle 50%
  l = __population_size__ * .25;
  r = __population_size__ * .75;  
  Serial.print("Top 25% -> 50% ");
  Serial.print(l);
  Serial.print(" ");
  Serial.println(r);    
  for (int i = 0; i < (__population_size__ * .3); i++) {
    random_index = random(l, r + 1);
    Serial.print(random_index);
    Serial.print(" --> ");
    Serial.println(target);    
    copy_genome( population[random_index], candidates[target++] );
  }
  
  // 20% of candidates are drawn from the bottom 25%  
  l = __population_size__ * .75;
  r = __population_size__;
  Serial.print("Top 25% -> 50% ");
  Serial.print(l);
  Serial.print(" ");
  Serial.println(r);    
  for (int i = 0; i < (__population_size__ * .2); i++) {
    random_index = random(l, r + 1);
    Serial.print(random_index);
    Serial.print(" --> ");
    Serial.println(target);    
    copy_genome( population[random_index], candidates[target++] );    
  }
  Serial.println(" ");  
}

void mutate_candidates() {
  for (int i = 0; i < __population_size__; i++) {
    mutate(candidates[i]);
  }
}

void recombine_candidates_into_population() {
  byte tmp;
  int target = 0;
  for (int i = 0; i < (__population_size__ / 2); i++) {
    int candidateX = random(0, __population_size__);
    int candidateY = random(0, __population_size__);
    
    copy_genome( candidates[candidateX], population[target++] );
    copy_genome( candidates[candidateY], population[target++] );
        
    int posA = random(0, __genome_length__);
    int posB = random(0, __genome_length__);
    
    int locusA = posA / __length__;
    int traitA = posA % __traits__;

    int locusB = posB / __length__;
    int traitB = posB % __traits__;

    bool recomb = false;
    for (int locus = 0; locus < __length__; locus++) {
      for (int trait = 0; trait < __traits__; trait++) {
        if ((locus == locusA || locus == locusB) && (trait == traitA || trait == traitB)) {
          recomb = !recomb;
        }
        
        if (recomb) { // swap the values
          tmp = population[target - 2][locus][trait];
          population[target - 2][locus][trait] = population[target - 1][locus][trait];
          population[target - 1][locus][trait] = tmp;
        }
      }
    }    
  }  
}

void select_and_mutate() {
  select_candidates();
  mutate_candidates();
  recombine_candidates_into_population();
}

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
  int target_index = __population_size__ - 1;
  int l, r, r_adj, samplesize; 
  // 10% of candidates from the bottom 20%
  l = (__population_size__ * 0.2);
  r = __population_size__;
  r_adj = r;
  samplesize = __population_size__ * 0.1;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  // 40% of the candidates from the middle 60%
  l = (__population_size__ * 0.2);
  r = (__population_size__ * 0.8);
  r_adj = r;
  samplesize = __population_size__ * 0.4;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  // 60% of candidates are from the top 20%
  l = 0;
  r = (__population_size__ * 0.2);
  r_adj = r;
  samplesize = __population_size__ * 0.4;
  for (int i = 0; i < samplesize && target_index >= 0; i++, target_index--) {  
    if (r_adj >= target_index) { r_adj = target_index - 1; }
   
    random_index = random(l, r_adj);
    copy_genome(population[random_index], population[target_index]);
  }
  
  // MUTATE
  for (int i = 0; i < __population_size__; i++) {
    mutate(population[i]);
  }

  // RECOMBINE n/2 times.
  byte tmp;
  for (int i = 0; i < (__population_size__ / 2); i++) {
    int candidateX = random(0, __population_size__);
    int candidateY = random(0, __population_size__);
    
    int posA = random(0, __genome_length__);
    int posB = random(0, __genome_length__);
    
    bool recomb = false;
    for (int j = 0; j < __genome_length__; j++) {
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




