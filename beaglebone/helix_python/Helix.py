##############################################
# EvoLight Open Pixel Control Python Interface
##############################################
import time
from optparse import OptionParser

## this is the data we're messing with
from EvoController import EvoController

## this is our inflows and outflows
from InterfaceController import InterfaceController

from DebugLog import DebugLog

class Helix:
    """
    Wrapper class to enforce the "with" syntax for initializing the class

    example:
        ## fetch the args
        (options, args) = parser.parse_args()

        with Helix(options, args) as helix:
            helix.do_process_loop()
    """
    def __init__(self, options, args):
        self.options = options
        self.args = args

    def __enter__(self):

        class HelixGuts:
            def __init__(self, options, args):

                self.options = options
                self.args = args

                self.debug_handle = DebugLog()

                self.evo = EvoController(
                    mut_rate=options.mutationrate,
                    population_size=options.populationsize,
                    locus_count=options.length,
                    debug=options.debug,
                    debug_handle=self.debug_handle)

                self.interface = InterfaceController(
                      opc_server=options.opc_server,
                      opc_port=options.opc_port,
                      use_opc=(not options.sim),
                      interpol=options.interpol,

                      ## Curses Display
                      use_curses=options.curses,

                      ## Call-backs
                      reset_ga_hook=self.reset_ga,
                      get_colors_hook=self.get_colors,
                      get_status_hook=self.get_status,
                      quit_hook=self.quit,

                      ## Debug Handle
                      debug_handle=self.debug_handle
                    )

            def init(self):
                self.debug_handle.init()
                self.interface.init()

            def perform_evolution(self):
                if (self.evo.generation > 10000):
                    self.evo.reset_ga()

                self.evo.next()

            def reset_ga(self):
                self.evo.reset_ga()

            def get_colors(self):
                data = [[(locus.r, locus.g, locus.b) for locus in org.genome]
                          for org in self.evo.allorg]

                if options.all:
                    return data
                if options.orgct:
                    return data[:options.orgct]
                else:
                    return data[:1]



            def get_status(self):
                return str(self.evo)

            def quit(self):
                exit()

            def cleanup(self):
                """
                Perform closing cleanups. Right now, this just means telling the
                interface to stop curses, etc.
                """
                self.interface.cleanup()

            def do_process_loop(self):
                ## activity loop
                while True:
                    if self.options.endat and self.evo.generation > self.options.endat:
                        self.quit()

                    self.interface.input_process()
                    self.perform_evolution()
                    self.interface.output_process()

                    ## TODO, remove this and have it sit elsewhere
                    time.sleep(1/options.fps)

        self.helix = HelixGuts(self.options, self.args)
        self.helix.init()
        return self.helix

    def __exit__(self, type, value, traceback):
        self.helix.cleanup()
        print "REALLY QUITTING" ## nothing to see here

if __name__ == '__main__':

    # Set up options
    usage = """usage: %prog [options]
    """
    parser = OptionParser(usage)
    parser.add_option("-d", "--debug_messages", action="store_true",
                      dest="debug", default=False,
                      help="print debug messages to stdout")
    parser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                      default=False, help="display verbose evolution information.")
    parser.add_option("-c", "--curses", action="store_true", dest="curses",
                      default=False, help="provide a curses display output")

    parser.add_option("--length", dest="length", type="int",
                      default=30, help="The length of the organism")
    parser.add_option("--popsize", dest="populationsize", type="int",
                      default=30, help="Population size")
    parser.add_option("--mutrate", dest="mutationrate", type="float",
                      default=0.01, help="Mutation rate per site")

    parser.add_option("-s", "--server", type="string", dest="opc_server",
                      default="127.0.0.1",
                      help=("The address of the Open Pixel Control server. "+
                      "Default 127.0.0.1"))
    parser.add_option("-p", "--port", type="string", dest="opc_port",
                      default="7890",
                      help=("The port of the open Pixel Control server. " +
                      "Default: 7890"))
    parser.add_option("--fps", type="int", default=20,
                      help="Frames per second. Default 20.")

    parser.add_option("--all", action="store_true", dest="all", default=False,
                      help="Display the whole population")
    parser.add_option("--orgct", type="int", dest="orgct", default=1,
                      help="Number of organisms to display")
    parser.add_option("--opc_interpolate", action="store_true", dest="interpol",
                      default=False, help="display all organisms on one string.")

    parser.add_option("--simulation", action="store_true", dest="sim",
                      default=False, help="do not send to the lights")

    parser.add_option("--endat", dest="endat", type="int",
                      help="Run for some number of generations and then quit.")

    ## fetch the args
    (options, args) = parser.parse_args()

    ## run helix
    with Helix(options, args) as helix:
        helix.do_process_loop()
