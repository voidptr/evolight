## Defaults

import opc

class OPCConnection:
    def __init__(self, server, port):
        self.server = server
        self.port = port

    def init(self):
        self.client = opc.Client(self.server + ":" + self.port)
        print "TODO"
        if self.client.can_connect():
            print "Connected to %s" % self.server + ":" + self.port
        else:
            print "Couldn't connect to %s" % self.server + ":" + self.port
            print "Aborting..."
            exit(1)

    def send_lights(self, lights, channels=[0]):
        """
        Parameters
            lights - array of arrays of pixel tuples (R,G,B). Each sub-array goes to a channel.

            channels - array of channel #s
        """
        for (light_string,channel) in zip(lights, channels):
            #print light_string
            #print channel
            self.client.put_pixels(light_string, channel)

    ### OPC operations -- think about moving this elsewhere
    def display(self, lights):
        channels = range(len(lights))
        self.send_lights([lights], channels)

    def interpolated(self, alllights):
        locusct = len(alllights)
        lights = []

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
                    interlights = self.OPC_fade_intermediary(old_lights, lights, frac/fracval)
                self.send_lights([interlights], [0]);
                #time.sleep(.5)
            #print "."

    def OPC_fade_intermediary(self, old_lights, lights, fracval):
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
