//
//  GA.cpp
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "GA.h"

GA::GA()
{
    
    // init the content of the Populations Array
    int PopulationSizes[] = __POPULATION_SIZES__;
    
    int startIndex = 0;
    for (int i = 0; i < __POPULATIONS__; i++)
    {
        Populations[i].StartIndex = startIndex;
        Populations[i].Count = PopulationSizes[i];
        
        startIndex += PopulationSizes[i];
    }
    
    current_mutation_rate = per_site_mutation_rate;
    
    use_heredity = true;
    use_selection = true;
    generation = true; 
}

double GA::per_site_mutation_rate = __DEFAULT_MUTATION_RATE__;

void GA::init()
{
    // init the random seed
    randomSeed(analogRead(0));
    
    //debug_all();
    init_random_population();
    //debug_all();
    //delay(2000);
}

void GA::evolve()
{
    //migrate(); // if there is any migration between populations, do it now.
    
    for (int i = 0; i < __POPULATIONS__; i++)
    {
        //Debug::debugln("EVOLVE");
        //Debug::debugln("INITIAL");
        //debug_population(i);
        select_candidates(i);
        //Debug::debugln("SELECT");
        //debug_population(i);
        //debug_candidates();
        mutate_candidates(i);
        //Debug::debugln("MUTATE");
        //debug_population(i);
        //debug_candidates();
        reproduce_candidates(i);
        //Debug::debugln("REPRODUCE");
        //debug_population(i);
        sort_population(i);
        //Debug::debugln("SORT");
        //debug_population(i);
        //debug_population__top_organism(i);
    }
    //debug_bestfitness();
    //Debug::debugln(generation);
    generation++;
}

void GA::sort_population(int popIndex)
{
    comb_sort_pop( Populations[popIndex].StartIndex, Populations[popIndex].Count );
}

void GA::comb_sort_pop(int start, int n)
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
            if (Organisms[i + start].inverse_fitness - Organisms[i + start + gap].inverse_fitness > 0) {
                swap.clone( Organisms[i + start] );
                Organisms[i + start].clone( Organisms[i + start + gap] );
                Organisms[i + start + gap].clone( swap );
                
                swapped = true;
            }
        }
    }
}

void GA::debug_all()
{
/*
    for (int i = 0; i < __TOTAL_ORGANISMS__; i++)
    {
        Organisms[i].debug();
    }
    Debug::debugln(" "); */
}

void GA::debug_candidates()
{
/*
    for (int i = 0; i < __LARGEST_POPULATION__; i++)
    {
        Candidates[i].debug();
    }
    Debug::debugln(" ");
    */
}

void GA::debug_population(int popIndex)
{
/*
    Debug::debug("Pop ");
    Debug::debug(popIndex);
    Debug::debug(" - Best Fitness: ");
    Debug::debugln( Organisms[Populations[popIndex].StartIndex].inverse_fitness );
    
    for (int i = 0; i < Populations[popIndex].Count; i++)
    {
        Organisms[Populations[popIndex].StartIndex + i].debug();
    }
    
    Debug::debugln(" "); */
}

void GA::debug_population__top_organism(int popIndex)
{
/*
    Debug::debug("Pop ");
    Debug::debug(popIndex);
    Debug::debug(" - Best Fitness: ");
    //Debug::debugln( Organisms[Populations[popIndex].StartIndex].inverse_fitness );
    Organisms[Populations[popIndex].StartIndex].debug();
    */
    /*
     for (int i = 0; i < Populations[popIndex].Count; i++)
     {
     Organisms[Populations[popIndex].StartIndex + i].debug();
     }
     */
    //Debug::debugln(" ");
}

void GA::debug_bestfitness()
{
/*
    int bestfit = 1000000;
    int bestpop = 0;
    for (int i = 0; i < __POPULATIONS__; i++)
    {
        if (Organisms[Populations[i].StartIndex].inverse_fitness < bestfit)
        {
            bestfit = Organisms[Populations[i].StartIndex].inverse_fitness;
            bestpop = i;
        }
    }
    
    Debug::debug("Pop ");
    Debug::debug(bestpop);
    Debug::debug(" - Best Fitness: ");
    Debug::debugln(bestfit);
    */
}


void GA::init_random_population()
{
    for (int i = 0; i < __TOTAL_ORGANISMS__; i++)
    {
        Organisms[i].randomize();
    }
}

void GA::randomize_population(int popIndex)
{
    for (int i = Populations[popIndex].StartIndex; i < Populations[popIndex].StartIndex + Populations[popIndex].Count; i++)
    {
        Organisms[i].randomize();
    }
}

bool GA::is_mut()
{
    int randval = random(0,999);
    if (per_site_mutation_rate * 1000 > randval)
    {
        //Debug::debug("mut");
        return true;
    }
    return false;
}

void GA::select_candidates(int popIndex)
{
    if (use_selection)
        //select_candidates__hybrid(popIndex);
        select_candidates__hybrid__evoluminator(popIndex);
    else
        select_candidates__random(popIndex);
}

void GA::mutate_candidates(int popIndex)
{
    for (int org = 0; org < Populations[popIndex].Count; org++)
    {
        Candidates[org].mutate();
    }
}

void GA::reproduce_candidates(int popIndex)
{
    if (use_heredity)
        recombine_candidates_into_population(popIndex);
    else
        randomize_population(popIndex);
}

void GA::recombine_candidates_into_population(int popIndex)
{    
    for (int i = 0; i < Populations[popIndex].Count; i++)
    {
        int candidate1 = random(0, Populations[popIndex].Count);
        int candidate2 = random(0, Populations[popIndex].Count);
        
        int childIndex = Populations[popIndex].StartIndex + i;
        
        BirthChamber[0].clone( Candidates[ candidate1 ] );
        BirthChamber[1].clone( Candidates[ candidate2 ] );
        
        Organism::recombine(BirthChamber[0], BirthChamber[1]);
        
        int randomindex = random(0, 2);
        //Debug::debugln(randomindex);
        Organisms[childIndex].clone( BirthChamber[ randomindex ] );

    }
}

// select randomly from the population
void GA::select_candidates__random(int popIndex)
{
    for (int i = 0; i < Populations[popIndex].Count; i++)
    {
        int candidateIndex = random(0, Populations[popIndex].Count);
        Candidates[i].clone(Organisms[Populations[popIndex].StartIndex + candidateIndex]);
    }
}

// select all candidates from the top 50%
void GA::select_candidates__elitism(int popIndex)
{
    for (int i = 0; i < Populations[popIndex].Count; i++)
    {
        int candidateIndex = random(0, Populations[popIndex].Count * .5); // only the top 50%
        Candidates[i].clone(Organisms[Populations[popIndex].StartIndex + candidateIndex]);
    }
}

void GA::select_candidates__hybrid__evoluminator(int popIndex)
{
    int popsize = Populations[popIndex].Count;
    int offset = Populations[popIndex].StartIndex;
    
    int l = 0, r = 0, random_index = 0, target = 0;
    
    // 50% of candidates are drawn from the top 25%
    l = 0;
    r = popsize * .25;
//    Debug::debug("Top 25% -> 50% ");
//    Debug::debug(l);
//    Debug::debug(" ");
//    Debug::debugln(r);
    for (int i = 0; i < (popsize * .5); i++) {
        random_index = random(l, r + 1) + offset;
//        Debug::debug(random_index);
//        Debug::debug(" --> ");
//        Debug::debugln(target);
        Candidates[target++].clone(Organisms[random_index]);
    }
    
    
    // 30% of candidates are drawn from the middle 50%
    l = popsize * .25;
    r = popsize * .75;
//    Debug::debug("Middle 50% -> 30% ");
//    Debug::debug(l);
//    Debug::debug(" ");
//    Debug::debugln(r);
    for (int i = 0; i < (popsize * .3); i++) {
        random_index = random(l, r + 1) + offset;
//        Debug::debug(random_index);
//        Debug::debug(" --> ");
//        Debug::debugln(target);
        Candidates[target++].clone(Organisms[random_index]);
    }
    
    // 20% of candidates are drawn from the bottom 25%
    l = popsize * .75;
    r = popsize - 1;
//    Debug::debug("Bottom 25% -> 20% ");
//    Debug::debug(l);
//    Debug::debug(" ");
//    Debug::debugln(r);
    for (int i = 0; i < (popsize * .2); i++) {
        if (target >= popsize) // avoid an overrun
            break;
        
        random_index = random(l, r + 1) + offset;
//        Debug::debug(random_index);
//        Debug::debug(" --> ");
//        Debug::debugln(target);
        Candidates[target++].clone(Organisms[random_index]);
    }
//    delay(1000);
}

void GA::select_candidates__hybrid(int popIndex)
{
    // 50% are drawn from top 20%
    int target = 0;
    for (int i = 0; i < (int)(Populations[popIndex].Count * .5); i++, target++)
    {
        int popCandidate = random(0, Populations[popIndex].Count * .2) - 1;
        Candidates[target].clone(Organisms[Populations[popIndex].StartIndex + popCandidate]);
    }
    // 30% are drawn from middle 60%
    for (int i = 0; i < (int)(Populations[popIndex].Count * .3); i++, target++)
    {
        int popCandidate = random(0, (int)(Populations[popIndex].Count * .6)) +
            (int)(Populations[popIndex].Count * .2) - 1;
        Candidates[target].clone(Organisms[Populations[popIndex].StartIndex + popCandidate]);
    }
    // 20% are drawn from bottom 20%
    for (int i = 0; i < (int)(Populations[popIndex].Count * .2); i++, target++)
    {
        int popCandidate = random(0, (int)(Populations[popIndex].Count * .2)) +
            (int)(Populations[popIndex].Count * .8) - 1;
        Candidates[target].clone(Organisms[Populations[popIndex].StartIndex + popCandidate]);
    }
}

////////////////////////
// ORGANISMS SUB CLASS
////////////////////////
GA::Organism::Organism()
{
    randomize();
    calculate_fitness();
}

void GA::Organism::calculate_fitness()
{
    // sum of differences across the genome
    int sumDiff = 0;
    for (int i = 0; i < __LOCUS_COUNT__ - 1; i++)
    {
        sumDiff += Trait::calculate_difference(Traits[i], Traits[i+1]);
    }
    sumDiff += Trait::calculate_difference(Traits[0], Traits[__LOCUS_COUNT__ - 1]);
    
    // sum of the colorfulness across the genome
    int sumColor = 0;
    for (int i = 0; i < __LOCUS_COUNT__ - 1; i++)
    {
        sumColor += Traits[i].calculate_colorfulness();
    }
    int invSumColor = _MAX_COLORFULNESS_ - sumColor;
    
    inverse_fitness = (sumDiff * 4) + invSumColor;
}

void GA::Organism::randomize()
{
    for (int i = 0; i < __LOCUS_COUNT__; i++)
    {
        Traits[i].randomize();
    }
    calculate_fitness();
}

int GA::Organism::CompareTo(Organism & aOrg)
{
    if (inverse_fitness < aOrg.inverse_fitness)
        return -1;
    if (inverse_fitness > aOrg.inverse_fitness)
        return 1;
    return 0;
}

void GA::Organism::mutate()
{
    for (int i = 0; i < __LOCUS_COUNT__; i++)
    {
        Traits[i].mutate();
    }
}

void GA::Organism::mutate(int index)
{
    Traits[index].mutate();
}

void GA::Organism::recombine(Organism & organism, Organism & organism_2)
{
    int posA; //= random_generator.Next(__loci_count__ - 1);
    int posB; //= random_generator.Next(__loci_count__ - 1);
    
    bool recomb = false;
    for (int i = 0; i < __LOCUS_COUNT__; i++)
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

void GA::Organism::clone(Organism & aOriginal)
{
    for (int i = 0; i < __LOCUS_COUNT__; i++)
    {
        Traits[i].clone(aOriginal.Traits[i]);
    }
    inverse_fitness = aOriginal.inverse_fitness;
}

void GA::Organism::debug()
{
/*
    Debug::debug(inverse_fitness);
    Debug::debug(":");
   
    for (int i = 0; i < __LOCUS_COUNT__; i++)
    {
        Debug::debug(" ");
        Traits[i].debug();
    }
    Debug::debugln(" "); */
}
