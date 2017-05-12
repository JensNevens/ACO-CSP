//
//  ant.hpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#ifndef ant_hpp
#define ant_hpp

#include <stdio.h>

#include "csp.hpp"

class Ant {
    
    long* seed;               /* the random seed */
    long int* string;         /* the solution string */
    long int string_distance; /* the quality of the solution string */
    long int string_length;   /* how much of the solution has been build */
    
    double* selection_prob;   /* probability array for selecting a letter */
    double** probability;     /* pointer to colony pheromone */
    CSP *csp;
    long int m;
    long int l;
    long int n;
    
    bool acs;                 /* bool to indicate ACS */
    double q0;                /* exploration probability */
    
    void computeStringDistance();
    void clearString();
    long int getNextLetter();
    long int getProbLetter();
    void printString();
    
public:
    Ant();
    Ant(CSP *csp_arg, double **prob_info, long int *pseed);
    Ant(CSP *csp_arg, double **prob_info, long int *pseed, double q);
    ~Ant();
    Ant(Ant const& other);
    
    void freeAnt();
    
    /* Construct a solution */
    void Search();
    /* Returns the idx in the alphabet of the letter at position idx in the ant's solution */
    long int getLetter(long int idx);
    /* Returns the max distance between the ant's solution string and all strings in the CSP set */
    long int getStringDistance();
    /* Local Search on the current solution */
    void LocalSearch(double b_rep);
    /* Local pheromone update rule of ACS */
    void LocalPheromoneUpdate(double** pheromone, double rho, double initial_pheromone);
    
    
};

#endif /* ant_hpp */
