//
//  ControlPanelController.h
//  Helix
//
//  Created by Rosangela Canino-Koning on 6/17/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#include "IDEIncludes.h"

#ifndef __Helix__ControlPanelController__
#define __Helix__ControlPanelController__

#define __PORTS_PER_BOARD__ 10
#define __BOARD_COUNT__ 3

#define __MUTATION_SLIDER__ 0
#define __INHERITANCE_SLIDER__ 1
#define __SELECTION_SLIDER__ 2

#define __MUTATION_SLIDER_SEGMENT_CT__ 7
#define __INHERITANCE_SLIDER_SEGMENT_CT__ 7
#define __SELECTION_SLIDER_SEGMENT_CT__ 7

#define __MUTATION_SLIDER_START_PORT__ 0
#define __INHERITANCE_SLIDER_START_PORT__ 0
#define __SELECTION_SLIDER_START_PORT__ 0

#define __RESET_BUTTON__ 0
#define __PLAYPAUSE_BUTTON__ 1
#define __STEP_BUTTON__ 2

#define __RESET_BUTTON_PORT__ 9
#define __PLAYPAUSE_BUTTON_PORT__ 9
#define __STEP_BUTTON_PORT__ 9



#include "TouchController.h"

class ControlPanelController
{
private:

    TouchController Touch;
    
    byte Addresses[__BOARD_COUNT__];
    int RawStatus[__BOARD_COUNT__];

    bool StatusByPort[__BOARD_COUNT__][__PORTS_PER_BOARD__];
    
    int ResetCount;
    int StepCount;
    int PlayPauseCount;
    
    int MutCount;
    int InhCount;
    int SelCount;
            
public:
    ControlPanelController();
    void init();
    void update();
    int MutationSliderStatus();
    int InheritanceSliderStatus();
    int SelectionSliderStatus();
    
    int SliderStatus(int aBoard, int aStart, int aCount);
    
    bool ResetButtonStatus();
    bool PlayPauseButtonStatus();
    bool StepButtonStatus();
        
    void debug_loop();
};

#endif /* defined(__Helix__ControlPanelController__) */
