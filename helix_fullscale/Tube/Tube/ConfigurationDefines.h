//
//  ConfigurationDefines.h
//  Helix
//
//  Created by Rosangela Canino Koning on 1/30/13.
//  Copyright (c) 2013 Sube Studio. All rights reserved.
//

#ifndef ConfigurationDefines_h
#define ConfigurationDefines_h

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


#define __NUM_STRANDS__ 9
//       0  1 2 3 4 5 6 7 8 9
// POP1  [][][][][][][][][][]
//       10
// POP2  [][][][][][][][][][]
//       20
// POP3  [][][][][][][][][][]
//       30
// POP4  [][][][][][][][][][]
//       40
// POP5  [][][][][][][][][][]
//       50
// POP6  [][][][][][][][][][]
//       60
// RING  []
//       61
// INT   []
//       62
// CONT  ..
//       62
// RESET []
//       63 4 5 6 7 8
// MUTSL [][][][][][]
//       69 0 1 2 3 4
// INHSL [][][][][][]
//       75 6
// PLAYP [][]
//       77
// STEPL []
//       78
// SELSL [][][][][][]
//                          -----populations-----  int ring control
#define __LIGHT_PINS__       {2,  4,  5,  11, 12, 13, 25, 26, 27}
#define __LIGHTS_AT_PINS__   {10, 10, 10, 10, 10, 10, 1,   1, 22}
//                          0   10  20  30  40  50  60  61  62
#define __STARTING_INTERNAL_LIGHTS__ 60
#define __STARTING_RING_LIGHTS__ 61

#define __STARTING_CONTROL_PANEL__ 62
#define __RESET_LIGHT__ __STARTING_CONTROL_PANEL__ // 63
#define __MUT_SLIDER_START__ __RESET_LIGHT__ + 1 // 64
#define __INH_SLIDER_START__ __MUT_SLIDER_START__ + 6 // 70
#define __PLAY_LIGHT__ __INH_SLIDER_START__ + 6 // 76
#define __PAUSE_LIGHT__ __PLAY_LIGHT__ + 1 // 77
#define __STEP_LIGHT__ __PAUSE_LIGHT__ + 1 // 78
#define __SEL_SLIDER_START__ __STEP_LIGHT__ + 1 // 79

#define __LONGEST_STRAND__ 22
#define __TOTAL_LIGHTS__ 84

#define __BRIGHTNESS_SCALE__ 0.2

#endif //ConfigurationDefines_h

