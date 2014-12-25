//
//  ControlPanelController.cpp
//  Helix
//
//  Created by Rosangela Canino-Koning on 6/17/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "ControlPanelController.h"

ControlPanelController::ControlPanelController()
{
    for (int i = 0; i < __BOARD_COUNT__; i++)
    {
        Addresses[i] = 0;
        RawStatus[i] = 0;
    }
    
}

void ControlPanelController::init()
{
    Touch.init();
    for (int i = 0; i < __BOARD_COUNT__; i++)
    {
        Addresses[i] = Touch.FoundControllerAddresses[i];
        
    }
}

void ControlPanelController::update()
{
    char stringBuf[20];

    for (int i = 0; i < __BOARD_COUNT__; i++)
    {
        RawStatus[i] = Touch.fetch_sensor_status(Addresses[i]);

        for (int j = 0; j < __PORTS_PER_BOARD__; j++)
        {
            StatusByPort[i][j] = Touch.status_by_port(RawStatus[i], j);
        }
    }
}


//TODO fix this so that we don't repeat ourselves
int ControlPanelController::MutationSliderStatus()
{
    int current = SliderStatus(__MUTATION_SLIDER__, __MUTATION_SLIDER_START_PORT__, __MUTATION_SLIDER_SEGMENT_CT__);
    
    if (current > -1)
    {
        Debug::debug("MUT ");
        Debug::debugln(current);
        MutCount++;
        if (MutCount > 2)
        {
            Debug::debugln("DOING IT");
            MutCount = 0;
                        
            return current;
        }
    }
    else
    {
        MutCount = 0;
    }
    
    return -1;
}

int ControlPanelController::InheritanceSliderStatus()
{
    int current = SliderStatus(__INHERITANCE_SLIDER__, __INHERITANCE_SLIDER_START_PORT__, __INHERITANCE_SLIDER_SEGMENT_CT__);
    
    if (current > -1)
    {
        Debug::debug("INH ");
        Debug::debugln(current);        
        InhCount++;
        if (InhCount > 2)
        {
            Debug::debugln("DOING IT");
            InhCount = 0;
            
            return current;
        }
    }
    else
    {
        InhCount = 0;
    }
    
    return -1;

}

int ControlPanelController::SelectionSliderStatus()
{
    int current = SliderStatus(__SELECTION_SLIDER__, __SELECTION_SLIDER_START_PORT__, __SELECTION_SLIDER_SEGMENT_CT__);
    
    if (current > -1)
    {
        Debug::debug("SEL ");
        Debug::debugln(current);
        SelCount++;
        if (SelCount > 2)
        {
            Debug::debugln("DOING IT");
            SelCount = 0;
            
            return current;
        }
    }
    else
    {
        SelCount = 0;
    }
    
    return -1;
}

int ControlPanelController::SliderStatus(int aBoard, int aStart, int aCount)
{
    int topSegment = -1;
    int nextToTopSegment = -1;
    
    for (int i = aStart, ct = 0; i < aCount; i++, ct++)
    {
        if (StatusByPort[aBoard][i])
        {
            if (topSegment > -1)
            {
                nextToTopSegment = ct;
                break;
            }
            else
                topSegment = ct;
        }
            
    }

    // invert the values, because whatever
    if (topSegment > -1)
        topSegment = aCount - topSegment;
    
    if (nextToTopSegment > -1)
        nextToTopSegment = aCount - nextToTopSegment;
    
    
    // convoluted logic, but I think it's appropriate
    if ((nextToTopSegment > -1 && (nextToTopSegment == topSegment - 1)) || (topSegment == aCount - 1))
        return topSegment - 1;
    else
        return topSegment;
}

bool ControlPanelController::ResetButtonStatus()
{
    bool current = StatusByPort[__RESET_BUTTON__][__RESET_BUTTON_PORT__];
    
    if (current)
    {
        Debug::debugln("RESET");
        ResetCount++;
        if (ResetCount > 5)
        {
            Debug::debugln("DOING IT");
            ResetCount = 0;
            return current;
        }
    }
    else
    {
        ResetCount = 0;
    }
    
    return false;

}

bool ControlPanelController::PlayPauseButtonStatus()
{
    bool current = StatusByPort[__PLAYPAUSE_BUTTON__][__PLAYPAUSE_BUTTON_PORT__];
    
    if (current)
    {
        Debug::debugln("PLAYPAUSE");
        PlayPauseCount++;
        if (PlayPauseCount > 5)
        {
            Debug::debugln("DOING IT");
            PlayPauseCount = 0;
            return current;
        }
    }
    else
    {
        PlayPauseCount = 0;
    }
    
    return false;
}

bool ControlPanelController::StepButtonStatus()
{
    bool current = StatusByPort[__STEP_BUTTON__][__STEP_BUTTON_PORT__];
    
    if (current)
    {
        Debug::debugln("STEP");
        StepCount++;
        if (StepCount > 5)
        {
            Debug::debugln("DOING IT");
            StepCount = 0;
            return current;
        }
    }
    else
    {
        StepCount = 0;
    }
    
    return false;
}


void ControlPanelController::debug_loop()
{
    update();
    
    if (ResetButtonStatus())
        Debug::debugln("RESET");

    if (PlayPauseButtonStatus())
        Debug::debugln("PLAY PAUSE");

    if (StepButtonStatus())
        Debug::debugln("PLAY PAUSE");
    
    int mut = MutationSliderStatus();
    int inh = InheritanceSliderStatus();
    int sel = SelectionSliderStatus();
    
    if (mut > -1)
    {
        Debug::debug("MUT: ");
        Debug::debugln(mut);
    }

    if (inh > -1)
    {
        Debug::debug("INH: ");
        Debug::debugln(inh);
    }
    
    if (sel > -1)
    {
        Debug::debug("SEL: ");
        Debug::debugln(sel);
    }
}