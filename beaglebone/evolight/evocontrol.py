##############################################
# EvoLight Open Pixel Control Python Interface
##############################################

import opc

import time
import random
from optparse import OptionParser

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

parser.add_option("-s", "--server", type="string", default="127.0.0.1", 
                  help="The address of the Open Pixel Control server. Default 127.0.0.1")
parser.add_option("-p", "--port", type="string", default="7890",
                  help="The port of the open Pixel Control server. Default: 7890")
parser.add_option("--fps", type="int", default=20,
                  help="Frames per second. Default 20.")

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

## create the OPC connection
client = opc.Client(options.server + ":" + options.port)
if client.can_connect():
    print "Connected to %s" % options.server + ":" + options.port
else:
    print "Couldn't connect to %s" % options.server + ":" + options.port
    print "Aborting..."
    exit(1)

## defaults for processing
__color_offset__ = 16
__ticks__ = 256
__busy_loop_time__ = 8000

## init the population manager
pm = evolight.populationmanager()

def perform_activity():
    pm.evolve()
    organism = pm.selectfittest()

    client.put_pixels(organism.colors, channel=0)

## activity loop
while True:
    perform_activity()
    time.sleep(1/options.fps)






