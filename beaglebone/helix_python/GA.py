import random
import sys
import copy

class Trait:
    def __init__(self, r=0, g=0, b=0, random=False):
        self.r = r
        self.g = g
        self.b = b

        if random:
            self.randomize()

    # not exactly pythonic, but fuck you.
    def __repr__(self):
        return "(%d, %d, %d)" % (self.r, self.g, self.b)

    def __str__(self):
        return "(%d, %d, %d)" % (self.r, self.g, self.b)

    @property
    def mutation_rate(self):
        return GA.mutation_rate

    def is_mut(self):
        return GA.is_mut()


    def randomize(self):
        self.r = self.get_random()
        self.g = self.get_random()
        self.b = self.get_random()

    def mutate(self):
        if (self.is_mut()):
            #print " %d mr" % self.r
            self.r = self.get_random()

        if (self.is_mut()):
            #print " %d mg" % self.g
            self.g = self.get_random()

        if (self.is_mut()):
            #print " %d mb" % self.b
            self.b = self.get_random()

    @staticmethod
    def get_random():
        return random.randint(0, 255)

    @staticmethod
    def calculate_difference(trait1, trait2):
        return (abs(trait1.r - trait2.r) +
               abs(trait1.g - trait2.g) +
               abs(trait1.b - trait2.b))

    def calculate_colorfulness(self):
        return (abs(self.r - self.g) +
               abs(self.b - self.b) +
               abs(self.b - self.r))


class Organism:
    def __init__(self, genome_length=30):
        self.genome = [Trait(random=True) for ct in range(genome_length) ]

        self.inverseFitness = sys.maxint
        #print self.genome
        self.calculate_fitness()

    def __str__(self):
        return str(self.inverseFitness) + ", " + str(self.genome)

    def calculate_fitness(self):
        maxColorfulness = len(self.genome) * 255 * 2

        ## difference/similarity
        sumDiff = 0
        for i in range(len(self.genome) - 1):
            sumDiff += Trait.calculate_difference(self.genome[i], self.genome[i+1])
        sumDiff += Trait.calculate_difference(self.genome[0], self.genome[-1])

        ## colorfulness
        sumColor = sum([ trait.calculate_colorfulness() for trait in self.genome ])
        invSumColor = maxColorfulness - sumColor

        ## combine
        ## TODO, add strength of selection to individual things like colorfulness vs
        ## smoothness. 4/1 is fine for now.
        self.inverseFitness = (sumDiff * 4) + invSumColor

    def mutate(self):
        for trait in self.genome:
            trait.mutate()

    @staticmethod
    def recombine(org1, org2):
        """
        Two-fold cost of sex recombination. Only the single recombined offspring
        will be return.
        """
        #TODO - add a flexible max for the recombination loci
        __max_recombination_points = 10
        points = [ random.randint(0, len(org1.genome))
            for i in range(0, random.randint(0,__max_recombination_points))]
        points.sort()

        #print org1
        #print org2

        neworg = copy.deepcopy(org1)
        recomb = False
        for pos in range(len(org1.genome)):
            if pos in points:
                recomb = not recomb

            if recomb:
                neworg.genome[pos] = copy.deepcopy(org2.genome[pos])

        neworg.calculate_fitness()

        #print
        #print org1
        #print org2
        #print neworg
        #print

        return neworg

class GA:
    mutation_rate = 0

    def __init__(self,
                 mut_rate=0.01,
                 population_size=30,
                 locus_count=30, debug=False, verbose=False):
        self.generation = 0
        GA.mutation_rate = mut_rate ## dunno if this will work

        self.population = [ Organism() for ct in range(population_size) ]
        self.candidates = [] ## just putting this out there

        self.debug = debug
        self.verbose = verbose

    def __str__(self):
        return ("Gen: %d, BestFit: %d" % (self.generation, self.population[0].inverseFitness))

    @property
    def bestorg(self):
        return self.population[0]

    @property
    def allorg(self):
        return self.population


    def sort_population(self):
        self.population.sort(key=lambda org: org.inverseFitness)

    def reset_ga(self):
        self.population = [ Organism() ] * population_size
        self.generation = 0
        ## TODO, worry about the mutation rate reset later

    @staticmethod
    def is_mut():
        if GA.mutation_rate * 1000 > random.randint(0, 999):
            return True
        return False

    ## there's a few different ways. TODO
    def select_candidates(self):
        """
        Selection
        """
        self.select_candidates_hybrid_evoluminator()

    def select_candidates_hybrid_evoluminator(self):
        ## assuming the population list is sorted by inverse fitness.
        popsize = len(self.population)

        self.candidates = []

        # 50% of candidates are drawn from the top 25%
        self.get_candidates(0, (popsize * .25) - 1, popsize * .5);
        # 30% of candidates are drawn from the middle 50%
        self.get_candidates(popsize * .25, (popsize * .75) - 1, popsize * .3);
        # 20% of candidates are drawn from the bottom 25%
        self.get_candidates(popsize * .75, popsize - 1, popsize * .2);

    def get_candidates(self, segment_start, segment_end, candidate_count):
        for i in range(int(candidate_count)):
            self.candidates.append(self.population[
                random.randint(int(segment_start), int(segment_end))])

    def mutate_candidates(self):
        """
        Mutation
        """
        for candidate in self.candidates:
            candidate.mutate()

    def reproduce_candidates(self):
        """
        Reproduction
        TODO: add other reproduction methods.
        """
        self.recombine_candidates_into_population()

    def recombine_candidates_into_population(self):
        for i in range(len(self.population)):
            self.population[i] = Organism.recombine(
                self.candidates[random.randint(0, len(self.candidates) - 1)],
                self.candidates[random.randint(0, len(self.candidates) - 1)])

    def evolve(self):

        if self.verbose:
            print self

        if self.debug:
            print "Debugging - %d" % self.generation
            print "Initial Population - "
            self.debug_population()

        self.select_candidates()
        if self.debug:
            print "Population After Selection - "
            self.debug_population()
            print "Candidates After Selection - "
            self.debug_candidates()

        self.mutate_candidates()
        if self.debug:
            print "Population After Mutation - "
            self.debug_population()
            print "Candidates After Mutation - "
            self.debug_candidates()

        self.reproduce_candidates()
        if self.debug:
            print "Population After Recombination - "
            self.debug_population()
            print "Candidates After Recombination - "
            self.debug_candidates()

        self.sort_population()
        if self.debug:
            print "Sorted Population - "
            self.debug_population()

        self.generation += 1

    def debug_population(self):
        for org in self.population:
            print org

    def debug_candidates(self):
        for org in self.candidates:
            print org







