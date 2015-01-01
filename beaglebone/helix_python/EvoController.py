import GA

## currently this class is pretty empty, and pointless-seeming
## eventually, it will include the interface controls in here, but there aren't any yet.

class EvoController:
    def __init__(self, debug=False, verbose=False):
        #self.GA = GA_Tournament.GA_Tournament()
        self.GA = GA.GA(debug=debug, verbose=verbose)

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


