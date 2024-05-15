import curses
from WindowPref import WindowPref


class CursesController:
    def __init__(self, debug_handle=None):

        self.debug_handle = debug_handle

        ## defaults for processing
        self.__color_offset__ = 16
        self.__ticks__ = 256
        self.__busy_loop_time__ = 8000

        self.__color_dictionary__ = {}

        ## set up the curses environment
        self.stdscr = curses.initscr()
        if not (curses.has_colors() and curses.can_change_color()):
            curses.echo()
            curses.endwin()
            print "Your terminal does not support color changing. Sorry."
            return false

        self.stdscr.timeout(0)

        curses.start_color()
        curses.use_default_colors()
        self.debug_handle.log("colors: %d color_pairs: %d" % (curses.COLORS, curses.COLOR_PAIRS))

        self.__bottom, self.__right = self.stdscr.getmaxyx()

        ## windows

        ### obsv, the whole window
        self.whole_window = WindowPref(left=0,
                                  right=self.__right,
                                  top=0,
                                  bottom=self.__bottom,
                                  debughook=self.debug_handle.log)

        ### runs along the top, anchored to the whole window edges
        ### top/left/right on edges
        ### bottom is 4 down from the top edge
        ### bordered
        self.title_window = WindowPref(left=self.whole_window.pos.left,
                                          right=self.whole_window.pos.right,
                                          top=self.whole_window.pos.top,
                                          bottom=self.whole_window.pos.top + 4,
                                          has_border=True,
                                          windowname="title_window",
                                          debughook=self.debug_handle.log,
                                          mainwindow=self.stdscr)

        ### runs along the bottom, anchored to the whole window edges
        ### bottom/left/right on edges
        ### top is 3 up from the bottom edge
        ### bordered
        ### TODO, make this be inverse colored, sans-border
        self.status_box = WindowPref(left=self.whole_window.pos.left,
                                      right=self.whole_window.pos.right,
                                      top=self.whole_window.pos.bottom - 3,
                                      bottom=self.whole_window.pos.bottom,
                                      has_border=True,
                                      windowname="status_box",
                                      debughook=self.debug_handle.log,
                                      mainwindow=self.stdscr)

        ### runs along the bottom, above the status window
        ### left/right are anchored to the whole_window
        ### bottom is anchored to the top of the status window
        ### top is 3 up from the top of the status window
        ### bordered
        self.input_box = WindowPref(left=self.whole_window.pos.left,
                                      right=self.whole_window.pos.right,
                                      top=self.status_box.pos.top - 3,
                                      bottom=self.status_box.pos.top,
                                      has_border=True,
                                      windowname="input_box",
                                      debughook=self.debug_handle.log,
                                      mainwindow=self.stdscr)

        ### sits in the middle, between the input box and the title box
        ### left/right are anchored to the whole_window
        ### top anchored to bottom of the title window
        ### bottom anchored to the top of the input box
        self.main_window = WindowPref(left=self.whole_window.pos.left,
                                      right=self.whole_window.pos.right,
                                      top=self.title_window.pos.bottom,
                                      bottom=self.input_box.pos.top,
                                      has_border=False,
                                      windowname="main_window",
                                      debughook=self.debug_handle.log,
                                      mainwindow=self.stdscr)

    def init(self):

        self.title_window.init()
        self.status_box.init()
        self.input_box.init()
        self.main_window.init()

        self.prepare_window()

    def cleanup(self):
        curses.endwin()

    ## fill in the space where the organism will be displayed
    def prepare_window_depr(self):
        i = 0
        for y in range(self.main_window.height):
            for x in range(self.main_window.width):
                pair_num = i + 1
                curses.init_pair(pair_num, 0, 0)
                #self.debug_handle.log("pn: %d" % i)
                self.main_window.draw_text(str(i%10), attr=curses.color_pair(pair_num), left=x, top=y)
                i += 1
        self.debug_handle.log("PrepWindow: %d" % i)

    def display_depr(self, color_tuples):
        self.debug_handle.log("DISPLAY")
        #self.debug_handle.log("w: %d h: %d -- tw: %d th: %d" % (
        #    self.main_window.width, self.main_window.height,
        #    self.main_window.text_width, self.main_window.text_height))
        for y in range(len(color_tuples)):
            if y >= self.main_window.height:
                break
            colors = color_tuples[y]
            for x in range(0, len(colors)):
                if x >= self.main_window.width:
                    break

                (R, G, B) = colors[x]
                R = int(((R)*8)/256) ## curses requires 0-1000 rather than 0-255
                G = int(((G)*8)/256)
                B = int(((B)*4)/256)

                colornum = (R << 5) | (G << 2) | B

                pair_num = (y * self.main_window.width) + x + 1

                #self.debug_handle.log("%d %d - %d %d (%d, %d, %d)" % (x, y, pair_num, colornum, R*125, G*125, B*250))

                curses.init_color(colornum, R*125, G*125, B*250) ## these are kept by curses.
                curses.init_pair(pair_num, colornum, colornum) ## this is the key bit
                #self.main_window.draw_text(str((pair_num - 1)%10), attr=curses.color_pair(pair_num), left=x, top=y)

    def prepare_window(self):
        """
        Pre-define all the colors and pairs
        Unfortunately, though you're probably in a terminal that supports lots
        of color_pairs (check terminfo), your ncurses is almost certainly not
        able to handle it. In fact, there seems to be a bug in python curses,
        where it tells you that curses.COLOR_PAIRS is more than 256, but it just
        modulos around. It's annoying.

        Per twobob's answer on this stack overflow question:
        (http://stackoverflow.com/questions/15105055/c-brightyellow-and-brightgreen-colors-ncurses),
        you need to recompile your curses with --enable-ext-colors

        Haven't tried it myself, so YMMV.

        """
        i = 0
        for y in range(self.main_window.height):
            for x in range(self.main_window.width):
                curses.init_pair(1, 0, 0)
                self.main_window.draw_text(" ", attr=curses.color_pair(1), left=x, top=y)
                i += 1
        self.debug_handle.log("PrepWindow - positions: %d" % i)

        for i in range(1, 256): # leave color 0 alone
            R = (i >> 5) & 0x7 ## top 3 bits
            G = (i >> 2) & 0x7 ## next 3 bits
            B = i & 0x3 ## bottom two bits

            R_val = R * 125
            G_val = G * 125
            B_val = B * 250

            curses.init_color(i, R_val, G_val, B_val)
            curses.init_pair(i, i, i)

    def display(self, color_tuples):

        for y in range(len(color_tuples)):
            if y >= self.main_window.height:
                break
            colors = color_tuples[y]
            for x in range(0, len(colors)):
                if x >= self.main_window.width:
                    break

                (R, G, B) = colors[x]
                R = int(((R)*8)/256) ## curses requires 0-1000 rather than 0-255
                G = int(((G)*8)/256)
                B = int(((B)*4)/256)
                colornum = (R << 5) | (G << 2) | B
                if colornum == 0:
                    colornum = 1

                self.main_window.chgat(1, attr=curses.color_pair(colornum), left=x, top=y, refresh=False)
                #self.main_window.draw_text(" ", attr=curses.color_pair(colornum), left=x, top=y, refresh=False)

        self.main_window.refresh()

    def set_title(self, title, message):
        ## Evolight Title Window
        self.title_window.draw_text_centered(title, top=1)
        self.title_window.draw_text_centered(message, top=2)
        #title_window.draw_text_centered("(+) to speed up, (-) to slow down.", top=3) ## causes crash. bleh.

    def status(self, message):
        self.status_box.draw_text(message)


    def prep_for_input(self):
        self.input_box.grab_cursor()

    def get_keypress(self):
        return self.stdscr.getch()

    def clear_input(self):
        self.input_box.clear()

    def refresh_display(self):
        self.stdscr.refresh()

