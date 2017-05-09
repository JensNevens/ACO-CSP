//
//  utils.cpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#include <cstdlib>

#include "utils.hpp"

/* constants for a pseudo-random number generator, details see Numerical Recipes in C book */
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836

double ran01( long *idum ) {
    /*
     FUNCTION:      returns a pseudo-random number
     INPUT:         a pointer to the seed variable
     OUTPUT:        a pseudo-random number uniformly distributed in [0,1]
     (SIDE)EFFECTS: changes the value of seed
     Notes:         call this function using ran01(&seed)
     */
    long k;
    double ans;
    
    k =(*idum)/IQ;
    *idum = IA * (*idum - k * IQ) - IR * k;
    if (*idum < 0 ) *idum += IM;
    ans = AM * (*idum);
    return ans;
}

/*** Pick a random number in range [min,max] or [min,max) ***/
long int pickRandom(long int min, long int max, long *seed, bool max_incl) {
    if (max_incl) {
        return min + (ran01(seed) * ((max - min) + 1));
    } else {
        return min + (ran01(seed) * (max - min));
    }
    
}

/*** Select a random element according to a probability density function ***/
int randomFromPDF(double *probs, int len) {
    double r = (double) rand() / (double) RAND_MAX;
    double cummulative = 0;
    int idx = -1;
    for (int i = 0; i < len; i++) {
        cummulative += probs[i];
        if (r <= cummulative) {
            idx = i;
            break;
        }
    }
    return idx;
}





