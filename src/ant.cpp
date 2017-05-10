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
    probability = prob_info;
    selection_prob = new double[m];
    string = new long int[l];
    string_distance = LONG_MAX;
    string_length = 0;
}

///* Copy constructor */
Ant::Ant(Ant const& other) {
    seed = other.seed;
    csp = other.csp;
    m = other.m;
    l = other.l;
    probability = other.probability;
    selection_prob = new double[m];
    string = new long int[l];
    string_distance = other.string_distance;
    string_length = 0;
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
    // Clear the currenc solution
    clearString();
    
    // Select first letter at random
    string[0] = (long int)(ran01(seed) * (double) m);
    string_length++;
    // Select each letter
    for (int i = 1; i < l; i++) {
        string[i] = getNextLetter();
        string_length++;
    }
    // Compute the quality of the string
    computeStringDistance();
    // printString();
}

/* Compute the distance of the string to the CSP set */
void Ant::computeStringDistance() {
    string_distance = csp->getDistance(string);
}

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

void move(long int* src, long int* dest, long int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

/* Local Search on the current solution.
   Replace each letter by every other possible letter
   and check if the score improves */
void Ant::LocalSearch() {
    // Create a copy of the current solution
    long int* copy = new long int[l];
    move(string, copy, l);
    // Find a better solution
    long int best_d = string_distance;
    for (int i = 0; i < l; i++) {
        long int current = string[i];
        long int replace = string[i];
        for (long int j = 0; j < m; j++) {
            if (j != current) {
                copy[i] = j;
                long int d = csp->getDistance(copy);
                if (d < best_d) {
                    best_d = d;
                    replace = j;
                }
            }
        }
        copy[i] = replace;
    }
    // If improvement, save as new solution
    if (best_d < string_distance) {
        move(copy, string, l);
        computeStringDistance();
    }
    delete [] copy;
}




