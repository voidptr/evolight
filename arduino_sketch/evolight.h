#include "utils.h"

/// evolution program setup
const char __length__ = 18;
const char __traits__ = 3;
const char __population_size__ = 10;
float __per_site_mutation_rate__ = 0.01;
const int __max_trait_val__ = 256;
const int __genome_length__ = __length__ * __traits__;
const float __selection_threshold__ = 0.25;

char population[__population_size__][__genome_length__]; // population
int fitnesses[__population_size__]; // index and fitness

void cycle_mutation_rate() {
  __per_site_mutation_rate__ = __per_site_mutation_rate__ * 2;
  
  if (__per_site_mutation_rate__ > 0.5) { __per_site_mutation_rate__ = 0.01; }
//  Serial.print("Mutation Rate: ");
//  Serial.println(__per_site_mutation_rate__); 
}

char get_rand_trait() {
  return char(random(0, 256));
}

int is_mut() {
  return random(0, 100) < __per_site_mutation_rate__ * 100;
}

void swap_genomes(int index1, int index2) {

  char tmp = 0;
  for (int i=0; i < __genome_length__; i++) {
    tmp = population[index1][i];
    population[index1][i] = population[index2][i];
    population[index2][i] = tmp;
  }
}

void comb_sort(int input[], int n) {
    
    int swap;
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

int calculate_fitness( char genome[] )
{
  int colordiff = 0, rdiff = 0, gdiff = 0, bdiff = 0;
  for (int i = 3; i < __genome_length__ - 2; i += 3) {
    rdiff = abs( genome[i-3] - genome[i] );
    gdiff = abs( genome[i-2] - genome[i+1] );
    bdiff = abs( genome[i-1] - genome[i+2] );   
   
    colordiff += (rdiff + gdiff + bdiff);    
  }
  
  return colordiff;
}

void calculate_fitnesses() {
  for (int i =0; i < __population_size__; i++) {
    fitnesses[i] = calculate_fitness( population[i] );
  }
  
  comb_sort(fitnesses, __population_size__);
}

void print_highestfitness() {
  Serial.println(fitnesses[0]);  
}

void init_random_population() {
 
  for (int i=0; i < __population_size__; i++) {  
    for (int j=0; j < __genome_length__; j++) {     
      population[i][j] = get_rand_trait();
    } 
  }
 
  calculate_fitnesses();
}

void reset_ga() {
  init_random_population();
}

// mutate a candidate genome. this is analogous to errors during gametogenensis. 
void mutate(char genome[]) { 
  for (int i = 0; i < __genome_length__; i++) {
    if (is_mut()) { // mutate
      genome[i] = get_rand_trait();
    }
  }
}

// this overwrites whatever's in the target slot with whatever the source was
void copy_genome(char source[], char target[]) {
//  Serial.println("copy_genome()");
  for (int i = 0; i < __genome_length__; i++) {
    target[i] = source[i];
  } 
}

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
}




