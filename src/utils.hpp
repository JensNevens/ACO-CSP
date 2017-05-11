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
void move(long int* src, long int* dest, long int size);
long int getMaxIdx(long int* arr, long int size);

#endif /* utils_hpp */
