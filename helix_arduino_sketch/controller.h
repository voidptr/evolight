#include "Arduino.h"

// processing
#include "evolight.h"
//#include "lights.h"
#include "lights_via_fastspi.h"
//#include "utils.h"

#ifndef DEBUG_H
#include "debug.h"
#define DEBUG_H
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class Controller 
{
private:
    int lights_num_rows;
    int lights_num_columns;
    int evo_population_size;
    int evo_loci_count;

    int rows;
    int columns;

public:
    GridLightControl * Lights;
    EvoLight * Evo;

public:
    // constructor, pass in the basic crap so we can pass it on to our minions.
    Controller() 
    {
        Lights = new GridLightControl();
        Evo = new EvoLight();

        debug("Controller CTOR");

        lights_num_rows = Lights->get_rows();
        lights_num_columns = Lights->get_columns();

        evo_population_size = Evo->get_population_size();
        evo_loci_count = Evo->get_loci_count();

        if (lights_num_rows < evo_population_size) 
        {
            rows = lights_num_rows;
        }
        else
        {
            rows = evo_population_size;
        }

        if (lights_num_columns < evo_loci_count) 
        {
            columns = lights_num_columns;
        }
        else
        {
            columns = evo_loci_count;
        }     
        debug("END Controller CTOR");

    }

    void init() 
    {
        debug("Controller init()");
       
        Lights->init();
        Evo->init();
        
        debug("END Controller init()");
    }

    void program_loop() 
    {
        debug("Controller program_loop()");
        Evo->evolve();
        display_population();
        debug("END Controller program_loop()");
    }
private:
    void display_population() // This is ostensibly the MVC paradigm. :/
    {
        debug("Controller display_population()");
        //Lights->debug_loop();
        Lights->clear();

        debug(Evo->get_highest_fitness());

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                EvoLight::Traits * trait = Evo->get_trait(i,j);
                Lights->set_light(i, j, trait->r, trait->g, trait->b);
            }
        } 
        Lights->show();
        delay(10);
        debug("END Controller display_population()");
    }

};

//extern Controller Control;
