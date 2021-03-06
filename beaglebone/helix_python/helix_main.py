##############################################
# EvoLight Open Pixel Control Python Interface
##############################################
import time
from optparse import OptionParser

from EvoController import EvoController
from OPCConnection import OPCConnection

# Set up options
usage = """usage: %prog [options]
"""
parser = OptionParser(usage)
parser.add_option("-d", "--debug_messages", action="store_true",
                  dest="debug", default=False,
                  help="print debug messages to stdout")
parser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                  default=False, help="display verbose evolution information.")

parser.add_option("--length", dest="length", type="int",
                  default=30, help="The length of the organism")
parser.add_option("--popsize", dest="populationsize", type="int",
                  default=30, help="Population size")
parser.add_option("--mutrate", dest="mutationrate", type="float",
                  default=0.01, help="Mutation rate per site")

parser.add_option("-s", "--server", type="string", default="127.0.0.1",
                  help="The address of the Open Pixel Control server. Default 127.0.0.1")
parser.add_option("-p", "--port", type="string", default="7890",
                  help="The port of the open Pixel Control server. Default: 7890")
parser.add_option("--fps", type="int", default=20,
                  help="Frames per second. Default 20.")

parser.add_option("--all", action="store_true", dest="all", default=False,
                  help="Display the whole population")

## fetch the args
(options, args) = parser.parse_args()

## init the OPC connection
conn = OPCConnection(options.server, options.port)
conn.init()

## init the evolution controller
evo = EvoController(
  mut_rate=options.mutationrate,
  population_size=options.populationsize,
  locus_count=options.length,
  debug=options.debug,verbose=options.verbose)

def perform_evolution():
    if (evo.generation > 10000):
        evo.reset_ga()

    evo.next()

def send_lights():
    if options.all:
        send_wholepop()
    else:
        send_single_org()


def send_single_org():
#    print evo.bestorg.genome
    lights = [(locus.r, locus.g, locus.b) for locus in evo.bestorg.genome]
    conn.send_lights([lights], [0])

def send_wholepop():
    locusct = len(evo.bestorg.genome)
    #print ">>>>"
    alllights = [[(locus.r, locus.g, locus.b) for locus in org.genome] for org in evo.allorg]
    lights = []
    #print "<<<<"
    #print
    for locus in range(locusct):
        old_lights = lights
        lights = []
        locus_offset = locus
        for org_idx in range(len(alllights)):
            lights.append(alllights[org_idx][locus_offset])
            locus_offset += 1
            if locus_offset >= locusct:
                locus_offset = 0

        fracval = 100.0
        #fracval = 2
        for frac in range(int(fracval)):
            if len(old_lights) == 0:
                interlights = lights
            else:
                interlights = fade_intermediary(old_lights, lights, frac/fracval)
            conn.send_lights([interlights], [0]);
            #time.sleep(.5)
        #print "."


def fade_intermediary(old_lights, lights, fracval):
    #print fracval
    #print lights
    #print old_lights
    interlights = []
    for i in range(len(old_lights)):
        interlights.append(
          (
            (lights[i][0] * fracval) + (old_lights[i][0] * (1 - fracval)),
            (lights[i][1] * fracval) + (old_lights[i][1] * (1 - fracval)),
            (lights[i][2] * fracval) + (old_lights[i][2] * (1 - fracval))
          ) )
    return interlights


## activity loop
while True:
    perform_evolution()
    send_lights()
    #time.sleep(1/options.fps)






