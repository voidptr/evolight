############################
# Evolight Python Interface
############################

import curses
from optparse import OptionParser

from WindowPref import WindowPref
from DebugLog import DebugLog
import evolight

# Set up options
usage = """usage: %prog [options]
"""
parser = OptionParser(usage)
parser.add_option("-d", "--debug_messages", action="store_true", dest="debug_messages",
                  default=False, help="print debug messages to stdout")

## fetch the args
(options, args) = parser.parse_args()

debug_handle = DebugLog()
debug_handle.init()

pm = evolight.populationmanager()

__color_offset__ = 16

def reset_colors( colortuples ):
    for i in range(0, len(colortuples)):
        (R, G, B) = colortuples[i]
        
        colornum = i + __color_offset__

        curses.init_color( colornum, R, G, B ) ## these are kept by curses.
        curses.init_pair( colornum, colornum, colornum ) ## this is the key bit

def handle_input(value):
    """ Handle the keypresses """

    pm.evolve()
    fittestorg = pm.selectfittest()

    reset_colors( fittestorg.colors )



    #input_box.clear()

    main_window.clear()
    main_window.draw_text( "Difference: %s Fitness %s" % \
                    (fittestorg.difference, fittestorg.fitness) )

    if value == ord('q'):
        curses.echo()
        curses.endwin()
        exit()
    elif value == ord('r'):
        pm.reset()
        

stdscr = curses.initscr()
stdscr.timeout(0)
curses.start_color()
curses.use_default_colors()

__bottom, __right = stdscr.getmaxyx()

## windows
whole_window = WindowPref(left=0,
                          right=__right,
                          top=0,
                          bottom=__bottom,
                          debughook=debug_handle.log)
status_bar = WindowPref(left=whole_window.pos.left,
                          right=whole_window.pos.right,
                          top=whole_window.pos.top,
                          bottom=whole_window.pos.top + 2,
                          windowname="status_bar",
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
                          top=status_bar.pos.bottom,
                          bottom=input_box.pos.top,
                          has_border=True,
                          windowname="main_window",
                          debughook=debug_handle.log,
                          mainwindow=stdscr)

main_window.init()
input_box.init()
status_bar.init()

#main_window.draw_text("Main Window!")
#status_bar.draw_text("Status Bar!")

for i in range(evolight.__length__):
    colornum = i + __color_offset__
    status_bar.draw_text("#", attr=curses.color_pair(colornum), left=i)

input_box.grab_cursor()

while True:
    stdscr.refresh()
    value = stdscr.getch()
    handle_input(value)
    input_box.grab_cursor()

curses.endwin()
