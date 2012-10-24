############################
# Evolight Python Interface
############################

import curses
import time
import random
from optparse import OptionParser

from WindowPref import WindowPref
from DebugLog import DebugLog
import evolight

# Set up options
usage = """usage: %prog [options]
"""
parser = OptionParser(usage)
#parser.add_option("-d", "--debug_messages", action="store_true", dest="debug_messages",
#                  default=False, help="print debug messages to stdout")
parser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                  default=False, help="display verbose evolution information.")

parser.add_option("--length", dest="length", type="int", help="The length of the organism")
parser.add_option("--popsize", dest="populationsize", type="int", help="Population size")
parser.add_option("--mutrate", dest="mutationrate", type="float", 
                  help="Mutation rate per site")

parser.add_option("--noblink", action="store_true", dest="noblink",
                  default=False, help="Don't do the blinking behavior")



## fetch the args
(options, args) = parser.parse_args()

## override the evolight defaults with parameters
if options.length:
    evolight.__length__ = options.length

if options.populationsize:
    evolight.__population_size__ = options.populationsize

if options.mutationrate:
    evolight.__persitemutationrate__ = options.mutationrate

## init the log
debug_handle = DebugLog()
debug_handle.init()

## defaults for processing
__color_offset__ = 16
__ticks__ = 256
__busy_loop_time__ = 8000

## init the colors so they aren't all black when we start up
def init_default_colors():
    for i in range(evolight.__length__):        
        colornum = i + __color_offset__
        curses.init_pair(colornum, -1, -1) ## this is the key bit

## set up the curses environment
stdscr = curses.initscr()
if not (curses.has_colors() and curses.can_change_color()):
    curses.echo()
    curses.endwin()
    print "Your terminal does not support color changing. Sorry."
    exit()


stdscr.timeout(0)
curses.start_color()
curses.use_default_colors()
init_default_colors()

__bottom, __right = stdscr.getmaxyx()

## windows
whole_window = WindowPref(left=0,
                          right=__right,
                          top=0,
                          bottom=__bottom,
                          debughook=debug_handle.log)
title_window = WindowPref(left=whole_window.pos.left,
                          right=whole_window.pos.right,
                          top=whole_window.pos.top,
                          bottom=whole_window.pos.top + 4,
                          has_border=True,
                          windowname="title_window",
                          debughook=debug_handle.log,
                          mainwindow=stdscr)
organism_bar = WindowPref(left=whole_window.pos.left,
                          right=whole_window.pos.right,
                          top=title_window.pos.bottom,
                          bottom=title_window.pos.bottom + 1,
                          windowname="organism_bar",
                          debughook=debug_handle.log,                          
                          mainwindow=stdscr)
input_box = WindowPref(left=whole_window.pos.left,
                          right=whole_window.pos.right,
                          top=whole_window.pos.bottom - 3,
                          bottom=whole_window.pos.bottom,
                          has_border=True,
                          windowname="input_box",
                          debughook=debug_handle.log,
                          mainwindow=stdscr)
main_window = WindowPref(left=whole_window.pos.left,
                          right=whole_window.pos.right,
                          top=organism_bar.pos.bottom,
                          bottom=input_box.pos.top,
                          has_border=False,
                          windowname="main_window",
                          debughook=debug_handle.log,
                          mainwindow=stdscr)

main_window.init()
title_window.init()
input_box.init()
organism_bar.init()

## Evolight Title Window
title_window.draw_text_centered("EVOLIGHT", top=1)
title_window.draw_text_centered("Press (r) to reset, (v) to toggle verbose mode, (b) to toggle blink, or (q) to quit.", top=2)
#title_window.draw_text_centered("(+) to speed up, (-) to slow down.", top=3) ## causes crash. bleh.

## init the population manager
pm = evolight.populationmanager()

## fill in the space where the organism will be displayed
for i in range(evolight.__length__):
    colornum = i + __color_offset__
    organism_bar.draw_text(" ", attr=curses.color_pair(colornum), left=i)

## define a global blink state
blink_state = [True for val in range(evolight.__length__)] ## all the lights start as on

## handle input/perform processing
def perform_activity(value):
    """ Handle the keypresses """

    pm.evolve()
    org = pm.selectfittest()

    if options.verbose:
        show_verbose_info(org)
 
    express_genome(org, sync=random.randint(0, 2))

    if value == ord('q'):
        show_verbose_info(org)
        time.sleep(2)
        curses.echo()
        curses.endwin()
        exit()

    elif value == ord('v'):
        input_box.clear()
        clear_verbose()
        options.verbose = not options.verbose
    elif value == ord('r'):
        input_box.clear()
        pm.reset()
    elif value == ord('b'):
        options.noblink = not options.noblink

## actually express the genome.
def express_genome(organism, sync=0):
    color_tuples = organism.colors

    if not options.noblink:
        for i in range(evolight.__length__):
            blink_state[i] = False

    ## first, set the colors to be expressed, respecting the blinkstate
    for i in range(0, len(color_tuples)):
        (R, G, B) = color_tuples[i]
        
        colornum = i + __color_offset__

        curses.init_color(colornum, R, G, B) ## these are kept by curses.

        use = colornum
        if not blink_state[i] and not options.noblink:
            use = -1
        curses.init_pair(colornum, use, use) ## this is the key bit

    if not options.noblink:
        ## perform the blink expression
        blink_transitions = [val/2 for val in organism.blinks] ## the number of ticks between transitions
        blink_ticks = [0 for val in organism.blinks] ## the number of ticks in the current state

        for tick in range(__ticks__ * 2):
            for light in range(0, len(color_tuples)):
                colornum = light + __color_offset__

                blink_ticks[light] += 1
                main_window.clear(line=10)
                if blink_ticks[light] == blink_transitions[light]:
                    blink_ticks[light] = 0 # reset the counter
                    if blink_state[light]:
                        blink_state[light] = False # light off
                        curses.init_pair(colornum, -1, -1)
                    else:
                        blink_state[light] = True # light on
                        curses.init_pair(colornum, colornum, colornum)
            busy = 0
            for i in range(__busy_loop_time__):
                busy += 1


def show_verbose_info(org):
    """Display verbose info about the organism and population."""
    main_window.clear(line=1)
    main_window.clear(line=2)
    main_window.clear(line=3)
    main_window.clear(line=4)
    main_window.clear(line=5)

    main_window.draw_text("Gen: %s" % pm.generation, top=1)
    main_window.draw_text("Color Fitness %s" % org.colorfitness, top=2)
    main_window.draw_text("Blink Sync Fitness %s" % org.blinksyncfitness, top=3)
    main_window.draw_text("Blink Speed Fitness %s" % org.blinkspeedfitness, top=4)
    main_window.draw_text("Total Fitness: %s" % org.fitness, top=5)

    main_window.clear(line=6)


def clear_verbose():
    main_window.clear(line=1)
    main_window.clear(line=2)
    main_window.clear(line=3)
    main_window.clear(line=4)
    main_window.clear(line=5)


## activity loop
input_box.grab_cursor()
while True:
    stdscr.refresh()
    inputval = stdscr.getch()
    perform_activity(inputval)
    input_box.grab_cursor()

curses.endwin()





