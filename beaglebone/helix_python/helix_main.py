#!/usr/local/bin/python
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
                  help="The length of the organism")
parser.add_option("--popsize", dest="populationsize", type="int",
                  help="Population size")
parser.add_option("--mutrate", dest="mutationrate", type="float",
                  help="Mutation rate per site")

parser.add_option("-s", "--server", type="string", default="127.0.0.1",
                  help="The address of the Open Pixel Control server. Default 127.0.0.1")
parser.add_option("-p", "--port", type="string", default="7890",
                  help="The port of the open Pixel Control server. Default: 7890")
parser.add_option("--fps", type="int", default=20,
                  help="Frames per second. Default 20.")

## fetch the args
(options, args) = parser.parse_args()

## init the OPC connection
conn = OPCConnection(options.server, options.port)
conn.init()

## init the evolution controller
evo = EvoController(debug=options.debug,verbose=options.verbose)

def perform_evolution():
    if (evo.generation > 10000):
        evo.reset_ga()

    evo.next()

def send_lights():
    send_single_org()

def send_single_org():
#    print evo.bestorg.genome
    lights = [(locus.r, locus.g, locus.b) for locus in evo.bestorg.genome]
    conn.send_lights([lights], [0])

## activity loop
while True:
    perform_evolution()
    send_lights()
    time.sleep(1/options.fps)






