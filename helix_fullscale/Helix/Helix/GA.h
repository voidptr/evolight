//
//  GA.h
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

// Core library for code-sense
#include "IDEIncludes.h"

#ifndef __Helix__GA__
#define __Helix__GA__

#include "ConfigurationDefines.h"
#include "Debug.h"

#include "Random.h"

#ifndef __GA_CONFIGURATION_DEFINES__
#define __GA_CONFIGURATION_DEFINES__
#define __POPULATIONS__ 6
#define __POPULATION_SIZES__ {29, 29, 29, 29, 29, 29}
#define __LOCUS_COUNT__ 10
#define __LARGEST_POPULATION__ 29 // this needs to be here to define
#define __TOTAL_ORGANISMS__ 66
#define __DEFAULT_MUTATION_RATE__ 0.01
#define __DEFAULT_PLASTICITY__ 0.0
#define __DEFAULT_SELECTION_STRENGTH__ 0.1

#define _MAX_TRAIT_VALUE_ 255
#endif


#define _MAX_COLORFULNESS_ __LOCUS_COUNT__ * _MAX_TRAIT_VALUE_ * 2

class GA
{
public:
    class Organism
    {
        struct Trait
        {
        public:
            uint8_t r, g, b, p_r, p_g, p_b;
            
            Trait()
            {
                r = 0;
                g = 0;
                b = 0;
                p_r = 0;
                p_g = 0;
                p_b = 0;
            }
            
            Trait(byte rVal, byte gVal, byte bVal)
            {
                r = rVal;
                g = gVal;
                b = bVal;
                p_r = get_phenotype(r);
                p_g = get_phenotype(g);
                p_b = get_phenotype(b);
            }
            
            void randomize()
            {
                r = get_random();
                g = get_random();
                b = get_random();
                p_r = get_phenotype(r);
                p_g = get_phenotype(g);
                p_b = get_phenotype(b);
            }
            
            void mutate()
            {
                if (is_mut())
                {
                    r = get_random();
                    p_r = get_phenotype(r);
                }
                if (is_mut())
                {
                    g = get_random();
                    p_g = get_phenotype(g);
                }
                if (is_mut())
                {
                    b = get_random();
                    p_b = get_phenotype(b);
                }
            }
            
            void clone(Trait aTrait)
            {
                r = aTrait.r;
                g = aTrait.g;
                b = aTrait.b;
                p_r = aTrait.p_r;
                p_g = aTrait.p_g;
                p_b = aTrait.p_b;
            }
            
            uint8_t get_phenotype(byte aVal)
            {
                return (uint8_t)(Random::normal(aVal, plasticity()));
            }
            
            uint8_t get_random()
            {
                return (uint8_t)(random(0, _MAX_TRAIT_VALUE_ + 1));
            }
            
            int calculate_colorfulness()
            {
                return abs(p_r - p_g) + abs(p_g - p_b) + abs(p_b - p_r);
            }
            
            static int calculate_difference(Trait aTraitA, Trait aTraitB)
            {
                return abs(aTraitA.p_r - aTraitB.p_r) +
                abs(aTraitA.p_g - aTraitB.p_g) +
                abs(aTraitA.p_b - aTraitB.p_b);
            }
            
            void debug()
            {
                Debug::debug(r);
                Debug::debug("-");
                Debug::debug(g);
                Debug::debug("-");
                Debug::debug(b);
                Debug::debug("  ");
                Debug::debug(p_r);
                Debug::debug("-");
                Debug::debug(p_g);
                Debug::debug("-");
                Debug::debugln(p_b);
            }
        };
        
    public:
        Trait Traits[__LOCUS_COUNT__];
        int inverse_fitness;
    public:
        Organism();
        void randomize();
        void calculate_fitness();
        int CompareTo(Organism& obj);
        void mutate();
        void mutate(int index);
        static void recombine(Organism& organism, Organism& organism_2);
        void clone(Organism& aOriginal);
        void debug();
    };
    
    struct Population
    {
    public:   
        uint16_t StartIndex;
        uint8_t Count;
    };
        
public:
    static double per_site_mutation_rate;
    static double selection_strength;
    //static double phenotypic_plasticity;
    static double heritability;
    
    //double current_mutation_rate;
    
    int generation;
    
    bool use_heredity;
    bool use_selection;
    
    Organism Organisms[__TOTAL_ORGANISMS__];
    Organism Candidates[__LARGEST_POPULATION__];
    Organism BirthChamber[2]; 
    Population Populations[__POPULATIONS__];
    
 
public:
	GA();
    void init();
    void comb_sort_pop(int start, int n);
    void sort_population(int popIndex);
    void init_random_population();
    void randomize_population(int popIndex);
    static bool is_mut();
    static double plasticity();
    void evolve();
    
    virtual void select_candidates(int popIndex);
    virtual void mutate_candidates(int popIndex);
    virtual void reproduce_candidates(int popIndex);
        
    void select_candidates__random(int popIndex);
    void select_candidates__elitism(int popIndex);
    void select_candidates__hybrid(int popIndex);
    void select_candidates__hybrid__evoluminator(int popIndex);
    
    void recombine_candidates_into_population(int popIndex);
    
    void reset_ga();
    void print_fitnesses();
    
    void debug_population(int popIndex);
    void debug_population__top_organism(int popIndex);
    void debug_all();
    void debug_candidates();
    void debug_bestfitness();





};

#endif /* defined(__Helix__GA__) */
