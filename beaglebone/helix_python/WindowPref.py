import curses
import math

## TODO - remove need for hook to the main window
class WindowPref:
    """
    Wrapper for Curses
    """
    class Edge:
        """
        A standard box class
        """
        def __init__(self, left=0, right=0, top=0, bottom=0):
            self.left = left
            self.right = right
            self.top = top
            self.bottom = bottom

    def __init__(self, left=0, right=0, top=0, bottom=0, bg=None,
            has_border=False, window=None, windowname=None, reverse=False,
            mainwindow=None,
            debughook=None):
        """
        Constructor
        """
        self.window = window
        self.pos = WindowPref.Edge(left, right, top, bottom)
        self.bg = bg  # color
        self.has_border = has_border
        self.debughook = debughook
        self.windowname = windowname
        self.mainwindow = mainwindow

        if self.has_border:
            self.textbox = WindowPref.Edge(1, right - left - 1, 1, bottom - top - 1)
        else:
            self.textbox = WindowPref.Edge(0, right - left, 0, bottom - top)

        self.debug("__init__")

    def __str__(self):
        return "<WindowPref pos.top:%s pos.bottom:%s pos.left:%s pos.right:%s has_border:%s windowname:%s>" % \
                (self.pos.top, self.pos.bottom, self.pos.left, self.pos.right, self.has_border, self.windowname)

    def init(self):
        """Actually initializes the window."""
        self.debug("init")

        self.window = curses.newwin(self.height, self.width, self.pos.top, self.pos.left)
        self.mainwindow.refresh()

        if self.has_border:
            self.window.border()

        self.refresh()

    def debug(self, message):
        if self.debughook:
            self.debughook(message)

    def clear(self, line=None):  # set up the basic shit, since we just cleared it.
        """Clear this window (or line)."""
        #self.debug("clear")

        if line:
            blanks = "".ljust(self.width, " ")
            self.draw_text(blanks, top=line)
        else:
            self.window.clear()
            if self.has_border:
                self.window.border()

        self.refresh()

    def refresh(self):
        """Perform a refresh on this window."""
        #self.debug("refresh")
        self.window.refresh()

    @property
    def height(self):
        return self.pos.bottom - self.pos.top

    @property
    def width(self):
        return self.pos.right - self.pos.left

    @property
    def text_width(self):
        return (self.textbox.right - self.textbox.left)

    @property
    def text_height(self):
        return (self.textbox.bottom - self.textbox.top)

    def _draw_line(self, text, attr=None, top=None, left=None):
        #self.debug("_draw_line text: %s attr:%s top:%s left:%s" % (text, attr, top, left))

        try:
            if attr:
                self.window.addstr(top, left, text, attr)
            else:
                self.window.addstr(top, left, text)
        except curses.error:
            pass

    def _draw_text_base(self, text, attr=None, top=None, left=None):
        if not top:
            top = self.textbox.top
        if not left:
            left = self.textbox.left

        line_width = self.text_width
        line_ct = len(text) / float(line_width)
        line_ct = int(math.ceil(line_ct))

        for i in range(0, line_ct):
            #self.debug("Draw Line! %d" % i)
            start = (line_width * i)
            end = (line_width * (i + 1))
            if end > len(text):
                end = len(text)

            line = text[start:end]
            self._draw_line(line, attr=attr, top=(top + i), left=left)

    def draw_text_centered(self, text, attr=None, top=None, refresh=True):
        """Draw some centered text."""
        centerpoint = self.width / 2
        half_text_length = len(text) / 2
        left = centerpoint - half_text_length

        self.draw_text(text, attr=attr, top=top, left=left, refresh=True)

    def draw_text(self, text, attr=None, top=None, left=None, refresh=True):
        """Draw some text."""
        self._draw_text_base(text, attr=attr, top=top, left=left)

        if refresh:
            self.refresh()

    def _chgat(self, length=1, attr=None, top=None, left=None):
        #self.debug("_draw_line text: %s attr:%s top:%s left:%s" % (text, attr, top, left))

        try:
            self.window.chgat(top, left, length, attr)
        except curses.error:
            pass

    def _chgat_base(self, length=1, attr=None, top=None, left=None):
        if not top:
            top = self.textbox.top
        if not left:
            left = self.textbox.left

        self._chgat(length, attr=attr, top=top, left=left)

    def chgat(self, length=1, attr=None, top=None, left=None, refresh=False):
        """Draw some text."""
        self._chgat_base(length, attr=attr, top=top, left=left)
        if refresh:
            self.refresh()

    def grab_cursor(self, top=0, left=0):
        """Grab the cursor and return it to this window."""
        #self.debug("grab_cursor")
        self.mainwindow.move(self.pos.top + self.textbox.top + top, self.pos.left + self.textbox.left + left)
