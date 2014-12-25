Evo Light is a long term project for bringing evolutionary light displays to various platforms. Primary subdirectories include:

###curses_demo

The initial python/curses implementation, testing the concept in a terminal.

###evoluminator_arduino_sketch

The first hardware implementation, using an Arduino Mega and a custom light and touch board. The touch system uses the CapSense library for capacitive touch using a pair of GPIO pins.

###helix_desktop

A small-scale implementation of the helix concept, powered by a Netduino for the evolutionary computation, and an Arduino Mega to control the lights. This implementation also included an IR remote to control the functions.

###helix_fullscale/Helix

The complete implementation for the large-scale installation. The primary controller was a Digilent ChipKit Max32, with 32Mb of ram.



