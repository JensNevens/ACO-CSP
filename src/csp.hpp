//
//  csp.hpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#ifndef csp_hpp
#define csp_hpp

#include <stdio.h>

class CSP {
    
    long int m;
    long int n;
    long int l;
    char *alphabet;
    char **set;
    
    long int hamming(char* entry, char* string);
    void printParameters();
    
public:
    CSP(const char *file_name, bool verbose);
    ~CSP();
    long int getAlphabetSize();  /* m: the size of the alphabet A */
    long int getSetSize();       /* n: the size of the string set S */
    long int getStringSize();    /* l: the length of each string s in S */
    
    long int getDistance(long int* solution);
    long int getDistance(long int* solution, long int i);
    long int* getAllDistances(long int* solution);
    
    long int getCount(long int i, long int j);
    
    char* solution2string(long int* solution);
    long int* string2solution(char* string);
    
    bool sameLetters(char* string, long int j, long int pos);
    bool sameLetters(long int i, long int j, long int pos);
    char getLetter(long int i, long int pos);

};

#endif /* csp_hpp */
