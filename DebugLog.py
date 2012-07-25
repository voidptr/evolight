import time


class DebugLog:
    def __init__(self, logfile="log.log"):
        self.filename = logfile

    def init(self):
        self.handle = open(self.filename, "a")
        self.handle.write("%s\n" % time.ctime())

    def log(self, text=""):
        self.handle.write("%s\n" % text)
        self.handle.flush()
