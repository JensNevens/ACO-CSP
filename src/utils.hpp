//
//  utils.hpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>

double ran01(long *idum);
long int pickRandom(long int min, long int max, long *seed, bool max_incl);
int randomFromPDF(double *probs, int len);

#endif /* utils_hpp */
