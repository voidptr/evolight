//
//  ConfigurationDefines.h
//  Helix
//
//  Created by Rosangela Canino Koning on 1/30/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#ifndef Helix_ConfigurationDefines_h
#define Helix_ConfigurationDefines_h

// this is a weird set of defines, because there's really no other easy
// way to set this stuff up at compile time rather than at run time.

// debug.h config defines
#define __DEBUG_CONFIGURATION_DEFINES__
#define __SERIAL_SPEED__ 9600
#define __OUTPUT_DEBUG_LEVEL__ 1

// GA.h config defines
#define __GA_CONFIGURATION_DEFINES__
// multipop settings
//#define __POPULATIONS__ 22
//#define __POPULATION_SIZES__ {20, 36, 32, 34, 100, 100, 76, 90, 100, 58, 100, 52, 52, 52, 46, 100, 86, 76, 70, 48, 20, 20}
//#define __LOCUS_COUNT__ 10
//#define __LARGEST_POPULATION__ 100 // this needs to be here to define the size of the candidate pool
//#define __TOTAL_ORGANISMS__ 1348

// single population settings
#define __POPULATIONS__ 1
#define __POPULATION_SIZES__ {22}
#define __LOCUS_COUNT__ 30
#define __LARGEST_POPULATION__ 22
#define __TOTAL_ORGANISMS__ 22

#define _MAX_TRAIT_VALUE_ 255
#define __DEFAULT_MUTATION_RATE__ 0.01
#define __DEFAULT_HERITABILITY__ 1.0
#define __DEFAULT_SELECTION_STRENGTH__ 1.0
#define __MAX_PLASTICITY__ 80

// LightController.h configuration items
#define __LIGHT_CONFIGURATION_DEFINES__
#define __ORIENTATION__ 0x01 // That is, linear


#define __NUM_STRANDS__ 23
//                          16   17  18  19  20  1   2   3   4   5    6    7   8   9    10  11   12  13  14  15
//                                      18  19
//                            3n  6n  5n  5n  16n  16n  10n 13n 16n  8n  16n  8n  8n  8n  7n  16n  13n 12n 11n 4n  b
// multipop
//#define __LIGHTS_AT_PINS__ {100, 86, 76, 70, 48, 20, 36, 32, 34, 100, 100, 76, 90, 100, 58, 100, 52, 52, 52, 30, 20, 20, 20}
// singlepop

#define __LIGHT_PINS__     {4,   5,   6,   7,   8,   9,   25,  37,  23,  22,  70,  71,  72,  73,  74,  76,  77,  78,  79,  80,  39,  41,  30}
#define __LIGHTS_AT_PINS__ {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}

#define __STARTING_INTERNAL_LIGHTS__ 2000
#define __STARTING_RING_LIGHTS__ 2100

#define __STARTING_CONTROL_PANEL__ 2200
#define __RESET_LIGHT__ __STARTING_CONTROL_PANEL__ // 2200
#define __MUT_SLIDER_START__ __RESET_LIGHT__ + 1 // 2201
#define __INH_SLIDER_START__ __MUT_SLIDER_START__ + 6 //2207
#define __PLAY_LIGHT__ __INH_SLIDER_START__ + 6 //2213
#define __PAUSE_LIGHT__ __PLAY_LIGHT__ + 1 //2214
#define __STEP_LIGHT__ __PAUSE_LIGHT__ + 1 //2215
#define __SEL_SLIDER_START__ __STEP_LIGHT__ + 1 //2216

#define __LONGEST_STRAND__ 100

// multipop
//  #define __TOTAL_LIGHTS__ 1368
// singlepop
#define __TOTAL_LIGHTS__ 2300

#define __BRIGHTNESS_SCALE__ 1.0

#endif //Helix_ConfigurationDefines_h