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

/* Move the contents of one array to another. Arrays have equal size */
void move(long int* src, long int* dest, long int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

/* Get the idx of the largest element in the array */
long int getMaxIdx(long int* arr, long int size) {
    long int max_idx = -1;
    long int max_val = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
            max_idx = i;
        }
    }
    return max_idx;
}




