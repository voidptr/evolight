##########################
# Evolight Classes
##########################

import random
import math

## Defaults
__length__ = 32 
__traits__ = 4
__population_size__ = 20
__per_site_mutation_rate__ = 0.01 
__max_trait_val__ = 256 # exclusive, vals 0-255

def __genome_length__():
    return __length__ * __traits__ # reset

class genome_manager:
    @staticmethod
    def get_rand_trait():
        return random.randrange(0, __max_trait_val__)

    @staticmethod
    def is_mut():
        return random.uniform(0, 1) < __per_site_mutation_rate__

    @staticmethod
    def build_random_genome():
        genome = []
        for i in range(__genome_length__()):
            genome.append(genome_manager.get_rand_trait())

        return genome

    @staticmethod
    def recombine(genome1, genome2):
        """Two-point crossover"""
        crossovers =  [random.randrange(0, __genome_length__()),
                       random.randrange(0, __genome_length__())]
        crossovers.sort()

        newgenome1 = genome1[0:crossovers[0]] + \
                     genome2[crossovers[0]:crossovers[1]] +\
                     genome1[crossovers[1]:] 

        newgenome2 = genome2[0:crossovers[0]] + \
                     genome1[crossovers[0]:crossovers[1]] +\
                     genome2[crossovers[1]:] 

        return (newgenome1, newgenome2)

    @staticmethod
    def mutate(genome):
        """Point mutations"""
        mutated = list(genome)

        random_index = [i for i in range(__genome_length__()) if (genome_manager.is_mut())]

        for index in random_index:
            mutated[index] = genome_manager.get_rand_trait() 

        return mutated


class populationmanager:
    def __init__(self):
        self.reset()

    def reset(self):
        self.population = [organism(genome_manager.build_random_genome()) for i in range(0, __population_size__)]
        self.generation = 0

    @staticmethod
    def organism_cmp(org1, org2):
        if org1.fitness < org2.fitness:
            return -1
        elif org1.fitness == org2.fitness:
            return 0
        return 1
            
    def select_candidates_elitism(self, threshold=.25):
        candidates = []
        ## 100% drawn from top threshold
        for i in range(0, int(__population_size__)):
            candidates.append(random.sample(self.population[0:int(__population_size__ * threshold)], 1)[0])

        return candidates

    def select_candidates_structured(self):
        candidates = []
        ## 50% drawn from top %25
        for i in range(0, int(__population_size__ * .5)):
            candidates.append(random.sample(self.population[0:int(__population_size__ * .25)], 1)[0])
        ## 40% drawn from next %50 
        for i in range(0, int(__population_size__ * .4)):
            candidates.append(random.sample(self.population[int(__population_size__ * .25):int(__population_size__ * .75)], 1)[0])
        ## 10% drawn from bottom %25                               
        for i in range(0, int(__population_size__ * .1)):
            candidates.append(random.sample(self.population[int(__population_size__ * .75):], 1)[0])

        return candidates

    def select_candidates_hybrid(self, threshold=.25):
        candidates = []
        ## 90% drawn from top %threshold
        for i in range(0, int(__population_size__ * .9)):
            candidates.append(random.sample(self.population[0:int(__population_size__ * threshold)], 1)[0])
        ## 10% drawn from next %50 
        for i in range(0, int(__population_size__ * .1)):
            candidates.append(random.sample(self.population[int(__population_size__ * threshold):], 1)[0])

        return candidates


    def evolve(self): 
        self.generation += 1

        #candidates = self.select_candidates_elitism(.1)
        #candidates = self.select_candidates_structured()
        candidates = self.select_candidates_hybrid(.25)

        newpop = []
        for i in range(0, __population_size__/2):
            pair = random.sample(candidates, 2)
            (newgenome1, newgenome2) = genome_manager.recombine(pair[0].genome, pair[1].genome)
            newpop.append(organism(genome_manager.mutate(newgenome1)))
            newpop.append(organism(genome_manager.mutate(newgenome2)))

        newpop.sort(cmp=populationmanager.organism_cmp)
        newpop.reverse()

        self.population = newpop

    def selectfittest(self):
        return self.population[0]


class organism:
    def __init__(self, genome):
        self.colors = []
        """RGB tuples"""

        self.blinks = []
        """The rate at which a light will blink"""

        for i in range(0, __genome_length__(), __traits__):
            
            color = (int(genome[i]), int(genome[i + 1]), int(genome[i+2]))
            self.colors.append(color)

            self.blinks.append(genome[i + 3])

        self.genome = genome

        self.calculate_fitness()

    def calculate_fitness(self):
        self.calculate_color_fitness()
        self.calculate_blink_synchrony_fitness()
        self.calculate_blink_speed_fitness()

        self.fitness = (self.colorfitness * 2) + self.blinksyncfitness + self.blinkspeedfitness

    def calculate_color_fitness(self):
        colordiff = 0
        for i in range(1, len(self.colors)):
            colordiff += abs(self.colors[i-1][0] - self.colors[i][0])**2
            colordiff += abs(self.colors[i-1][1] - self.colors[i][1])**2
            colordiff += abs(self.colors[i-1][2] - self.colors[i][2])**2

        max_color_diff = (__length__) * 3 * (__max_trait_val__ - 1)**2
        self.colorfitness = (max_color_diff - colordiff) / float(max_color_diff)

    def calculate_blink_synchrony_fitness(self):
        blink_diff = 0
        for i in range(1, len(self.blinks)):
            blink_diff += (abs((self.blinks[i-1]) - (self.blinks[i])))

        max_blink_diff = (__length__ - 1) * (((__max_trait_val__ - 1)))
        self.blinksyncfitness = (max_blink_diff - blink_diff) / float(max_blink_diff)

    def calculate_blink_speed_fitness(self):
        blink_speed = sum(self.blinks)

        max_blink_speed = (__length__) * (__max_trait_val__ - 1)

        self.blinkspeedfitness = blink_speed / float(max_blink_speed)



    
        

    
