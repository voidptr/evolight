import random
import math

__length__ = 32 
__traits__ = 4
__genomelength__ = __length__ * __traits__
__population_size__ = 20
__persitemutationrate__ = 100 
__maxtraitval__ = 256 # exclusive, vals 0-255

class genomemanager:
    @staticmethod
    def getrandtrait():
        return random.randrange( 0, __maxtraitval__ )

    @staticmethod
    def ismutated():
        return random.randrange(0, __persitemutationrate__) == 1

    @staticmethod
    def buildrandomgenome():
        genome = []
        for i in range( __genomelength__ ):
            genome.append( genomemanager.getrandtrait() )

        return genome

    @staticmethod
    def recombine(genome1, genome2):
        """Two-point crossover"""
        crossoverpts =  [ random.randrange( 0, __genomelength__ ),
                          random.randrange( 0, __genomelength__ ) ]
        crossoverpts.sort()

        newgenome1 = genome1[0:crossoverpts[0]] + \
                        genome2[crossoverpts[0]:crossoverpts[1]] +\
                        genome1[crossoverpts[1]:] 

        newgenome2 = genome2[0:crossoverpts[0]] + \
                        genome1[crossoverpts[0]:crossoverpts[1]] +\
                        genome2[crossoverpts[1]:] 

        return (newgenome1, newgenome2)

    @staticmethod
    def mutate(genome):
        """Point mutations"""
        mutated = list(genome)

        ## this is incredibly inefficient. fix later. TODO
        randomindexes = [ i for i in range(__genomelength__) if ( genomemanager.ismutated() ) ]

        for index in randomindexes:
            mutated[index] = genomemanager.getrandtrait()

        return mutated

class populationmanager:
    def __init__(self):
        self.reset()

    def reset(self):
        self.population = [ organism(genomemanager.buildrandomgenome()) for i in range(0, __population_size__) ]

    @staticmethod
    def organism_cmp( org1, org2 ):
        if org1.fitness < org2.fitness:
            return -1
        elif org1.fitness == org2.fitness:
            return 0
        return 1
            

    def evolve(self): # there is not fitness here yet, just random drift. TODO
        newpop = []
        for i in range(0, __population_size__/2):
            pair = random.sample( self.population[__population_size__/2:], 2 )
            (newgenome1, newgenome2) = genomemanager.recombine( pair[0].genome, pair[1].genome )
            newpop.append( organism( genomemanager.mutate(newgenome1)) )
            newpop.append( organism( genomemanager.mutate(newgenome2)) )

        newpop.sort(cmp=populationmanager.organism_cmp)

        self.population = newpop

    def selectfittest(self): # there is no fitness here, just random   
        #return random.sample( self.population, 1 )[0]
        return self.population[-1]


class organism:
    __maxdiff__ = (__length__ - 1) * 3 * __maxtraitval__
    """The maximum difference between the colors and traits"""

    def __init__(self, genome):
        self.colors = []
        """RGB tuples"""

        self.blinkrates = []
        """The rate at which a light will blink"""

        for i in range(0, __genomelength__, __traits__):
            
            color = ( int(genome[i] * 3.9), int(genome[i + 1] * 3.9), int(genome[i + 2] * 3.9) )
            self.colors.append( color )

            self.blinkrates.append( genome[i + 3] )

        self.genome = genome

        diff = 0
        for i in range(1, len(self.colors)):
            diff += abs(self.colors[i-1][0] - self.colors[i][0])
            diff += abs(self.colors[i-1][1] - self.colors[i][1])
            diff += abs(self.colors[i-1][2] - self.colors[i][2])

        self.difference = diff
        self.fitness = organism.__maxdiff__ - diff
