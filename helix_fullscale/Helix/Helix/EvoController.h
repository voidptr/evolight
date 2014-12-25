//
//  EvoController.h
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

// Core library for code-sense
#include "IDEIncludes.h"

#ifndef __Helix__EvoController__
#define __Helix__EvoController__

#include "ConfigurationDefines.h"

//#include "GA.h"
#include "GA_Tournament.h"

class EvoController
{
public:
    //GA_Tournament Engine;
    GA Engine;
    bool Play;
    int Step;
private:

    bool Mutation;
    bool MutationEvent;
    
    //double current_mutation_rate;
    double valid_mutation_rates[6];
    //double valid_phenotypic_plasticities[6];
    double valid_heritability_values[6];
    double valid_selection_strengths[6];
    
    int mut_rate_setting;
//    int pheno_plast_setting;
    int heritability_setting;
    int selection_setting;
    
public:
	EvoController();
    void init();
    void next();
    void toggle_pauseplay();
    void toggle_selection();
    void toggle_heredity();
    void take_step();
    void raise_mut_rate();
    void lower_mut_rate();
    void reset_ga();
    void set_mutation_rate(int aVal);
    void set_heritability(int aVal);
    void set_selection_strength(int aVal);
    
    int get_mutation_rate_setting();
    int get_heritability_setting();
    int get_selection_strength_setting();
    
    // accessors
    int generation();
    double per_site_mutation_rate();
    
};

#endif /* defined(__Helix__EvoController__) */
