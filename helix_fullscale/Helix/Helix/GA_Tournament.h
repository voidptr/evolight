//
//  GA_Tournament.h
//  Helix
//
//  Created by Rosangela Canino Koning on 2/7/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

// Core library for code-sense
#include "IDEIncludes.h"

#ifndef __Helix__GA_Tournament__
#define __Helix__GA_Tournament__

#include "ConfigurationDefines.h"
#include "Debug.h"
#include "GA.h"

#define __FRACTION__ 0.2


class GA_Tournament : public GA
{
public:
    virtual void select_candidates(int popIndex);
    virtual void mutate_candidates(int popIndex);
    virtual void reproduce_candidates(int popIndex);
    
private:
    void select_candidates__tournament(int popIndex);
    void select_candidates__tournament_random(int popIndex);
    void recombine_candidates_into_population__tournament(int popIndex);
    void randomize_population__tournament(int popIndex);
    
    void comb_sort_winners(int array1[], int array2[], int n);
};


#endif /* defined(__Helix__GA_Tournament__) */
