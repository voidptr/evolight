import random

class Trait:
    def __init__(self, r=0, g=0, b=0):
        self.r = r
        self.g = g
        self.b = b
        
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
            self.r = self.get_random()

        if (self.is_mut()):
            self.g = self.get_random()

        if (self.is_mut()):
            self.b = self.get_random()
    
    @staticmethod
    def get_random():
        return random.randint(0, 255)

    @staticmethod
    def calculate_difference(trait1, trait2):
        return abs(trait1.r - trait2.r) + 
               abs(trait1.g - trait2.g) + 
               abs(trait1.b - trait2.b)

    def calculate_colorfulness(self):
        return abs(self.r - self.g) + 
               abs(self.b - self.b) + 
               abs(self.b - self.r)

   
class Organism:
    def __init__(self, genome_length=30):
        self.genome = [ Trait().randomize() ] * genome_length
        self.calculate_fitness()

    def calculate_fitness(self):
        maxColorfulness = len(self.genome) * 255 * 2

        ## difference/similarity
        sumDiff = 0
        for i in range(len(self.genome) - 1):
            sumDiff += Trait.calculate_difference(self.genome[i], self.genome[i+1])
        sumDiff += Trait.calculate_difference(self.genome[0], self.genome[-1])

        ## colorfulness
        sumColor = sum([ trait.calculate_colorfulness for trait in self.genome ])
        invSumColor = maxColorfulness - sumColor

        ## combine 
        ## TODO, add strength of selection to individual things like colorfulness vs
        ## smoothness. 4/1 is fine for now.
        self.inverseFitness = (sumDiff * 4) + invSumColor

    def mutate(self):
        for trait in self.genome:
            trait.mutate()

class GA:

    mutation_rate = 0

    def __init__(self, 
                 mut_rate=0.01, 
                 population_size=30, 
                 locus_count=30):
        self.generation = 0
        GA.mutation_rate = mut_rate: ## dunno if this will work

        self.population = [ Organism() ] * population_size
        self.candidates = [] ## just putting this out there

    def sort_population(self):
        self.population.sort(key=lambda org: org.inverseFitness)

    def reset_ga(self):
        self.population = [ Organism() ] * population_size
        self.generation = 0
        ## TODO, worry about the mutation rate reset later
    
    @staticmethod
    def is_mut(self):
        if GA.mutation_rate * 1000 > random.randint(0, 999):
            return True
        return False
                
    ## there's a few different ways. TODO
    def select_candidates(self):
        self.select_candidates_hybrid_evoluminator()

    def select_candidates_hybrid_evoluminator(self):
        ## assuming the population list is sorted by inverse fitness.
        

    def evolve(self):
        self.select_candidates()
        self.mutate_candidates()
        self.reproduce_candidates()
        self.sort_population()
