//
//  GA_Tournament.cpp
//  Helix
//
//  Created by Rosangela Canino Koning on 2/7/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "GA_Tournament.h"

void GA_Tournament::select_candidates(int popIndex)
{
    if (use_selection)
        //select_candidates__hybrid(popIndex);
        select_candidates__tournament(popIndex);
    else
        select_candidates__tournament_random(popIndex);
}

void GA_Tournament::mutate_candidates(int popIndex)
{
    for (int org = 0; org < Populations[popIndex].Count * __FRACTION__; org++)
    {
        Candidates[org].mutate();
    }
}

void GA_Tournament::reproduce_candidates(int popIndex)
{
    if (use_heredity)
        recombine_candidates_into_population__tournament(popIndex);
    else
        randomize_population__tournament(popIndex);
}

void GA_Tournament::select_candidates__tournament(int popIndex)
{
    // randomly select the tournament participants
    int participants_count = Populations[popIndex].Count * (__FRACTION__ * 2);
    int Indexes[participants_count];
    int Wins[participants_count];
    for (int i = 0; i < participants_count; i++)
    {
        Indexes[i] = random(0, Populations[popIndex].Count);
        Wins[i] = 0;
    }
    
    for (int i = 0; i < participants_count * 5; i++)
    {
        int participant1 = random(0, participants_count);
        int participant2 = random(0, participants_count);
        
        if (Organisms[Populations[popIndex].StartIndex + Indexes[participant1]].inverse_fitness <
                Organisms[Populations[popIndex].StartIndex + Indexes[participant2]].inverse_fitness)
        {
            Wins[participant1]++;
        }
        else
        {
            Wins[participant2]++;
        }
    }
    
    comb_sort_winners(Wins, Indexes, participants_count);
    
    for (int i = 0, p = participants_count - 1; i < Populations[popIndex].Count * (__FRACTION__); i++, p--)
    {
        Candidates[i].clone(Organisms[Populations[popIndex].StartIndex + Indexes[p]]);
    }
}

// select randomly from the population
void GA_Tournament::select_candidates__tournament_random(int popIndex)
{
    for (int i = 0; i < Populations[popIndex].Count * (__FRACTION__ * 2); i++)
    {
        int candidateIndex = random(0, Populations[popIndex].Count);
        Candidates[i].clone(Organisms[Populations[popIndex].StartIndex + candidateIndex]);
    }
}

void GA_Tournament::recombine_candidates_into_population__tournament(int popIndex)
{
    // fraction of new members of the population
    for (int i = 0; i < Populations[popIndex].Count * (__FRACTION__); i++)
    {
        // randomly select the candidates to recombine. No assortive mating
        int candidate1 = random(0, Populations[popIndex].Count * (__FRACTION__));
        int candidate2 = random(0, Populations[popIndex].Count * (__FRACTION__));
        
        // random replacement
        int childIndex = random(0, Populations[popIndex].Count) + Populations[popIndex].StartIndex;
        
        BirthChamber[0].clone( Candidates[ candidate1 ] );
        BirthChamber[1].clone( Candidates[ candidate2 ] );
        
        Organism::recombine(BirthChamber[0], BirthChamber[1]);
        
        int randomindex = random(0, 2);
        Organisms[childIndex].clone( BirthChamber[ randomindex ] );        
    }
 }

void GA_Tournament::randomize_population__tournament(int popIndex)
{
    for (int i = 0; i < Populations[popIndex].Count * (__FRACTION__); i++)
    {
        // random replacement
        int childIndex = random(0, Populations[popIndex].Count) + Populations[popIndex].StartIndex;
        Organisms[childIndex].randomize();
    }
}

void GA_Tournament::comb_sort_winners(int array1[], int array2[], int n)
{
    int swap;    
    int i, gap = n;
    bool swapped = false;
    
    while ((gap > 1) || swapped) {
        if (gap > 1) {
            gap = (int)((double)gap / 1.247330950103979);
        }
        
        swapped = false;
        
        for (i = 0; gap + i < n; ++i) {
            if (array1[i] - array1[i + gap] > 0) {
                
                swap = array1[i];
                array1[i] = array1[i + gap];
                array1[i + gap] = swap;
                
                swap = array2[i];
                array2[i] = array2[i + gap];
                array2[i + gap] = swap;
                
                swapped = true;
            }
        }
    }
}