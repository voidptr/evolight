import GA

## currently this class is pretty empty, and pointless-seeming
## eventually, it will include more than just the GA engine, like other
## kinds of GA and some more sophisticated management of the operation of the GA

class EvoController:
    def __init__(self, mut_rate=0.01,
                 population_size=30,
                 locus_count=30,
                 debug=False,
                 debug_handle=None):
        #self.GA = GA_Tournament.GA_Tournament()
        self.GA = GA.GA(
            mut_rate=mut_rate,
            population_size=population_size,
            locus_count=locus_count,
            debug=debug,
            debug_handle=debug_handle)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        print ""

    def __str__(self):
        return str(self.GA)

    def next(self):
        self.GA.evolve()

    def reset_ga(self):
        self.GA.reset_ga()

    @property
    def generation(self):
        return self.GA.generation

    @property
    def bestorg(self):
        return self.GA.bestorg

    @property
    def allorg(self):
        return self.GA.allorg

    ## TODO
    #void toggle_pauseplay();
    #void toggle_selection();
    #void toggle_heredity();
    #void take_step();
    #void raise_mut_rate();
    #void lower_mut_rate();
    #void set_mutation_rate(int aVal);
    #void set_heritability(int aVal);
    #void set_selection_strength(int aVal);
    #
    #int get_mutation_rate_setting();
    #int get_heritability_setting();
    #int get_selection_strength_setting();
    #
    # # accessors
    #double per_site_mutation_rate();


