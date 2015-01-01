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


