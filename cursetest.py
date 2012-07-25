import curses

curses.initscr()
curses.start_color()
raise( BaseException(curses.COLOR_PAIRS) )
