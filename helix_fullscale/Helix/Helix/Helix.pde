///
/// @mainpage	Helix
/// @details	<#details#>
/// @n
/// @n
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Rosangela Canino Koning
/// @author	Sube Studio
/// @date	1/22/13 3:30 PM
/// @version	<#version#>
///
/// @copyright	© Rosangela Canino Koning, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///


///
/// @file	Helix.pde
/// @brief	Main sketch
/// @details	<#details#>
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
///
/// @author	Rosangela Canino Koning
/// @author	Sube Studio
/// @date	1/22/13 3:30 PM
/// @version	<#version#>
///
/// @copyright	© Rosangela Canino Koning, 2013
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
/// @n
///


#include "Wire.h"

// Core library for code-sense
#include "IDEIncludes.h"

// Include application, user and local libraries
#include "Controller.h"

Controller controller;

void setup() {
    controller.init();
}

void loop() {   
    controller.loop();
}
