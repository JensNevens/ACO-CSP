//
//  ant.cpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "ant.hpp"
#include "utils.hpp"

Ant::Ant() {}

/* Constructor */
Ant::Ant(CSP* csp_arg, double** prob_info, long int *pseed) {
    seed = pseed;
    csp = csp_arg;
    m = csp->getAlphabetSize();
    l = csp->getStringSize();
    n = csp->getSetSize();
    probability = prob_info;
    selection_prob = new double[m];
    string = new long int[l];
    string_distance = LONG_MAX;
    string_length = 0;
    acs = false;
}

Ant::Ant(CSP* csp_arg, double** prob_info, long int *pseed, double q) {
    seed = pseed;
    csp = csp_arg;
    m = csp->getAlphabetSize();
    l = csp->getStringSize();
    n = csp->getSetSize();
    probability = prob_info;
    selection_prob = new double[m];
    string = new long int[l];
    string_distance = LONG_MAX;
    string_length = 0;
    acs = true;
    q0 = q;
}

///* Copy constructor */
Ant::Ant(Ant const& other) {
    seed = other.seed;
    csp = other.csp;
    m = other.m;
    l = other.l;
    n = other.n;
    probability = other.probability;
    selection_prob = new double[m];
    string = new long int[l];
    string_distance = other.string_distance;
    string_length = 0;
    acs = other.acs;
    q0 = other.q0;
    for (int i = 0; i < l; i++) {
        string[i] = other.string[i];
    }
    for (int i = 0; i < m; i++) {
        selection_prob[i] = other.selection_prob[i];
    }
}

/* Destructor */
Ant::~Ant() {}

/* Free memory */
void Ant::freeAnt() {
    delete [] string;
    delete [] selection_prob;
}


/* Generate tour using probabilities */
void Ant::Search() {
    // Clear the current solution
    clearString();
    
    // Select first letter at random
    string[0] = (long int) (ran01(seed) * (double) m);
    string_length++;
    // Select each letter
    for (int i = 1; i < l; i++) {
        if (acs) {
            double choice = ran01(seed);
            if (choice < q0) {
                // Exploitation
                string[i] = getProbLetter();
            } else {
                // Biased Exploration
                string[i] = getNextLetter();
            }
        } else {
            string[i] = getNextLetter();
        }
        string_length++;
    }
    // Compute the quality of the string
    computeStringDistance();
    // printString();
}

/* Local pheromone update rule of ACS */
void Ant::LocalPheromoneUpdate(double** pheromone, double rho, double initial_pheromone) {
    for (int j = 0; j < l; j++) {
        long int i = getLetter(j);
        pheromone[i][j] =
            ((1.0 - rho) * pheromone[i][j]) + (rho * initial_pheromone);
    }
}

/* Compute the distance of the string to the CSP set */
void Ant::computeStringDistance() {
    string_distance = csp->getDistance(string);
}

/* Return the quality of the current string */
long int Ant::getStringDistance(){
    return(string_distance);
}

/* Cleans the string building structures */
void Ant::clearString() {
    for (int i = 0; i < l; i++) {
        string[i] = -1;
    }
    for (int i = 0; i < m; i++) {
        selection_prob[i] = 0;
    }
    string_length = 0;
}

/* Obtains the next letter to add according to the random proportional rule */
long int Ant::getNextLetter() {
    double sum_prob = 0.0;
    long int j = string_length;
    
    for (int i = 0; i < m; i++) {
        // Calculate the probabily of selecting each letter
        sum_prob = sum_prob + probability[i][j];
        selection_prob[i] = sum_prob;
    }
    
    // Choose a letter
    double choice = ran01(seed) * sum_prob;
    long int idx = 0;
    while (choice > selection_prob[idx]) idx++;
    return(idx);
}

/* Obtains the next letter with the highest probability */
long int Ant::getProbLetter() {
    long int letter = -1;
    double letter_prob = 0.0;
    long int j = string_length;
    
    for (long int i = 0; i < m; i++) {
        if (probability[i][j] > letter_prob) {
            letter = i;
            letter_prob = probability[i][j];
        }
    }
    return letter;
}

/* Get the letter in position i of the string */
long int Ant::getLetter(long int i) {
    return(string[i]);
}

void Ant::printString() {
    for (int i = 0; i < l; i++) {
        printf("%ld - ", string[i]);
    }
    printf("Total cost: %ld \n", string_distance);
    
}

/* Local Search on the current solution. */
void Ant::LocalSearch(double b_rep) {
    // Initialize distance arrays
    long int* dist = csp->getAllDistances(string);
    long int* dist_cpy = new long int[n];
    move(dist, dist_cpy, n);
    // Initialize the loop counter N, the current max distance
    // and the string representation of the current solution
    int N = (int) (l * m * b_rep);
    long int max_dist = string_distance;
    char* sol_string = csp->solution2string(string);
    // Start local search
    do {
        N -= 1;
        long int max_idx = getMaxIdx(dist, n);
        for (int j = 0; j < l; j++) {
            if (!csp->sameLetters(sol_string, max_idx, j)) {
                long int max = -1;
                for (int k = 0; k < n; k++) {
                    if (k != max_idx) {
                        if (csp->sameLetters(sol_string, k, j)
                            && !csp->sameLetters(max_idx, k, j)) {
                            dist[k] += 1;
                        } else if (!csp->sameLetters(sol_string, k, j)
                                   && csp->sameLetters(max_idx, k, j)) {
                            dist[k] -= 1;
                        }
                        if (max < dist[k]) {
                            max = dist[k];
                        }
                    }
                }
                if (max_dist > max) {
                    N = (int) ((l * m * b_rep) / 3);
                }
                if (max_dist >= max) {
                    max_dist = max;
                    sol_string[j] = csp->getLetter(max_idx, j);
                    dist[max_idx] -= 1;
                    move(dist, dist_cpy, n);
                } else {
                    move(dist_cpy, dist, n);
                }
            }
        }
    } while (N > 0);
    // Finalize local search
    long int* new_string = csp->string2solution(sol_string);
    move(new_string, string, l);
    string_distance = max_dist;
    // Free up memory
    delete [] dist;
    delete [] dist_cpy;
    delete [] sol_string;
    delete [] new_string;
}




