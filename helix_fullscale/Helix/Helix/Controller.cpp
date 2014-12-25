//
//  Controller.cpp
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "Controller.h"

void Controller::init()
{    
    Debug::debugln("Light Init");
    Lights.init();
    
    //Debug::debugln("Touch Init");
    //Panel.init();
    
    Debug::debugln("Evo Init");
    Evo.init();
    
    Debug::debugln("Init Complete");
}

void Controller::loop()
{
    // looping tasks
    Debug::debug(".");
    control_panel_actions();
    control_panel_lights();
    evolution_actions();
    
    output_lights__singlepop_shortorgs();
    //output_lights__singlepop();
    //output_lights__multipop();
    
    
    
    //Panel.debug_loop();
}

void Controller::control_panel_lights()
{

    int mut_slider = Evo.get_mutation_rate_setting();
    int inh_slider_setting = Evo.get_heritability_setting();
    int sel_slider = Evo.get_selection_strength_setting();
    
    //Debug::debug("____");
    //Debug::debugln(mut_slider);
    
    byte colors_mut[6][3] =
    {   { 0, 255, 0 },
        { 51, 204, 0 },
        { 102, 153, 0 },
        { 153, 102, 0 },
        { 204, 51, 0 },
        { 255, 0, 0 } };

    byte colors_sel[6][3] =
    {   { 0, 255, 0 },
        { 51, 204, 0 },
        { 102, 153, 0 },
        { 153, 102, 0 },
        { 204, 51, 0 },
        { 255, 0, 0 } };

    byte colors_inh[6][3] =
    {   { 255, 0, 0 },
        { 204, 51, 0 },
        { 153, 102, 0 },
        { 102, 153, 0 },
        { 51, 204, 0 },
        { 0, 255, 0 } };
    
    // populate the lights for the mutation slider
    for (int i = 0; i < 6; i++)
    {
        if (i <= mut_slider)
            Lights.set_light(__MUT_SLIDER_START__ + i, colors_mut[i][0], colors_mut[i][1], colors_mut[i][2]);
        else
            Lights.set_light(__MUT_SLIDER_START__ + i, 0, 0, 0);
    }

    // populate the lights for the inheritance slider (they are backward)
    for (int i = 0; i < 6; i++)
    {
        if ((6 - i) < inh_slider_setting)
            Lights.set_light(__INH_SLIDER_START__ + i, colors_sel[i][0], colors_sel[i][1], colors_sel[i][2]);
        else
            Lights.set_light(__INH_SLIDER_START__ + i, 0, 0, 0);
    }

    // populate the lights for the selection slider
    for (int i = 0; i < 6; i++)
    {
        if (i < sel_slider)
            Lights.set_light(__SEL_SLIDER_START__ + i, colors_inh[i][0], colors_inh[i][1], colors_inh[i][2]);
        else
            Lights.set_light(__SEL_SLIDER_START__ + i, 0, 0, 0);
    }
    
 
    // set the slider values
    
    
    if (Evo.Play)
    {
        // TODO flash the play light once
    }
    else
    {
        // enable the pause light
        if (Evo.Step > 0)
        {
            // TODO Flash the step light
        }
    }
 
    
}

void Controller::control_panel_actions()
{
    Panel.update();
    
    int mut = Panel.MutationSliderStatus();
    int inh = Panel.InheritanceSliderStatus();
    int sel = Panel.SelectionSliderStatus();
    
    if (Panel.ResetButtonStatus())
    {
        Evo.reset_ga();
        // flash the reset light a bunch of times
    }
    
    if (Panel.PlayPauseButtonStatus())
    {
        Evo.toggle_pauseplay();
    }
    
    if (Panel.StepButtonStatus())
    {
        Evo.take_step();
    }
    
    if (mut > -1)
    {
        Evo.set_mutation_rate(mut);
    }

    if (inh > -1)
    {
        Evo.set_heritability(inh);
    }
    
    if (sel > -1)
    {
        Evo.set_selection_strength(sel);
    }
     
    
}

void Controller::evolution_actions()
{
    if (Evo.generation() > 10000)
    {
        Debug::debug("Over 10k Generations! Resetting GA");
        Evo.reset_ga();
    }
    Evo.next();
    //Evo.Engine.debug_population__top_organism(0);
    Evo.Engine.debug_bestfitness();
    
    
}

void Controller::output_lights__singlepop_shortorgs()
{
    int lightnum = 0;
    
    // loop through the organisms and apply to the rod that goes with it.
    for (int org = 0; org < __TOTAL_ORGANISMS__; org++) // loop through each organism
    {
        int strand_length = Lights.lightsatpins[org];
        for (int lite = 0; lite < strand_length; lite++) // loop through each light on a given rod
        {
            int locus = lite % __LOCUS_COUNT__; // the locus to display
            
            // set the light and increment the position
            bool lightok = Lights.set_light(lightnum++,
                                            Evo.Engine.Organisms[org].Traits[locus].p_r,
                                            Evo.Engine.Organisms[org].Traits[locus].p_g,
                                            Evo.Engine.Organisms[org].Traits[locus].p_b);
            
            if (!lightok) // ok, no more lights available
            {
                break;
            }
            
        }
    }
    Lights.show();
    delay(3);
}

void Controller::output_lights__singlepop()
{
    for (int org = 0; org < __TOTAL_ORGANISMS__; org++)
    {
        for (int seq = 0; seq < __LOCUS_COUNT__; seq++)
        {
            int lightnum = (org * __LOCUS_COUNT__) + seq;

            bool lightok = Lights.set_light(lightnum,
                                            Evo.Engine.Organisms[org].Traits[seq].p_r,
                                            Evo.Engine.Organisms[org].Traits[seq].p_g,
                                            Evo.Engine.Organisms[org].Traits[seq].p_b);
            
            if (!lightok)
            {
                break;
            }
        }
    }
    Lights.show();
    delay(3);
}

void Controller::output_lights__multipop()
{    
    for (int seq = 0; seq < __LOCUS_COUNT__; seq++)
    {
        for (int frac = 0; frac < 10; frac++)
        {
            double fracval = (double)frac/10;
            for (int org = 0; org < __TOTAL_ORGANISMS__; org++)
            {
                int traitprev = ((seq + org) % __LOCUS_COUNT__);
                int trait = ((seq + org + 1) % __LOCUS_COUNT__);
                bool lightok = Lights.set_light__fade_intermediary(org,
                                                    Evo.Engine.Organisms[org].Traits[trait].r,
                                                    Evo.Engine.Organisms[org].Traits[trait].g,
                                                    Evo.Engine.Organisms[org].Traits[trait].b,
                                                    Evo.Engine.Organisms[org].Traits[traitprev].r,
                                                    Evo.Engine.Organisms[org].Traits[traitprev].g,
                                                    Evo.Engine.Organisms[org].Traits[traitprev].b,
                                                    fracval);
                if (!lightok)
                {
                    break;
                }
            }
            Lights.show();
            delay(3);
        }
    }
}
