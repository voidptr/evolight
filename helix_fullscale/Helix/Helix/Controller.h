//
//  Controller.h
//  Helix
//
//  Created by Rosangela Canino-Koning on 1/28/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//
// Core library for code-sense
#include "IDEIncludes.h"

#ifndef __Helix__Controller__
#define __Helix__Controller__

#include "ConfigurationDefines.h"

#include "Debug.h"
#include "EvoController.h"
#include "ControlPanelController.h"
#include "LightController.h"

class Controller
{
private:
    EvoController Evo;
    GridLightControl Lights;
    ControlPanelController Panel;
        
public:
    
    void init();
    
    void loop();
    void evolution_actions();
    void control_panel_actions();
    void control_panel_lights();
    void output_lights__singlepop();
    void output_lights__singlepop_shortorgs();
    void output_lights__multipop();
        
};

#endif /* defined(__Helix__Controller__) */
