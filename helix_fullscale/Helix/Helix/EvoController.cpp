//
//  EvoController.cpp
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "EvoController.h"

EvoController::EvoController()
{
    Play = true;
    Step = 1;
    
    Mutation = true;
    MutationEvent = false;
    
    //current_mutation_rate = Engine.per_site_mutation_rate;
    
    valid_mutation_rates = {0.01, 0.02, 0.04, 0.08, 0.16, 0.32};
    valid_heritability_values = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};
//    valid_phenotypic_plasticities = {0, 5, 10, 20, 40, 80};
    valid_selection_strengths = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};
    
    mut_rate_setting = 0; // min
//    pheno_plast_setting = 0;
    heritability_setting = 5; // max
    selection_setting = 5; // max
    
    
}

void EvoController::init()
{
    Engine.init();
}

void EvoController::next()
{
    if (Play)
    {
        Engine.evolve();

    }
    else
    {
        if (Step > 0)
        {
            Engine.evolve();
            Step--;
        }
    }
}

void EvoController::toggle_pauseplay()
{
    Play = !Play;
}


void EvoController::take_step()
{
    if (!Play) // this only makes sense in the context of being paused
        Step++;
}

void EvoController::set_mutation_rate(int aVal)
{
    Engine.per_site_mutation_rate = valid_mutation_rates[aVal];
    mut_rate_setting = aVal;
}

int EvoController::get_mutation_rate_setting()
{
    return mut_rate_setting;
}

void EvoController::set_heritability(int aVal)
{
    Engine.heritability = valid_heritability_values[aVal];
    heritability_setting = aVal;
}

int EvoController::get_heritability_setting()
{
    return heritability_setting;
}

void EvoController::set_selection_strength(int aVal)
{
    Engine.selection_strength = valid_selection_strengths[aVal];
    selection_setting = aVal;
}

int EvoController::get_selection_strength_setting()
{
    return selection_setting;
}

void EvoController::reset_ga()
{
    Play = true;
    Step = 1;
    
    Engine.per_site_mutation_rate = __DEFAULT_MUTATION_RATE__;
    Engine.heritability = __DEFAULT_HERITABILITY__;
    Engine.selection_strength = __DEFAULT_SELECTION_STRENGTH__;
    
    mut_rate_setting = 0;
    heritability_setting = 5;
    selection_setting = 5;
    
    Engine.reset_ga();
}

int EvoController::generation()
{
    return Engine.generation;
}

double EvoController::per_site_mutation_rate()
{
    return Engine.per_site_mutation_rate;
}



