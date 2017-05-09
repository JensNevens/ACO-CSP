//
//  csp.cpp
//  ACO-CSP
//
//  Created by Jens Nevens on 8/05/17.
//  Copyright © 2017 Jens Nevens. All rights reserved.
//

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "csp.hpp"

/* Constructor */
CSP::CSP(const char *file_name) {
    // Create an ifstream
    std::ifstream infile(file_name);
    if (!infile.is_open()) {
        std::cerr << "Failed to open " << file_name << '\n';
        exit(1);
    }
    printf("\nReading csp-file %s ... \n", file_name);
    
    // Read alphabet length (m), set size (n) and string length (l)
    infile >> m;
    infile >> n;
    infile >> l;
    
    // Initialize alphabet array and set array
    alphabet = new char[m];
    set = new char * [n];
    
    // Read the alphabet characters
    for (int i = 0; i < m; i++) {
        infile >> alphabet[i];
    }
    
    // Read the set of strings
    for (int i = 0; i < n; i++) {
        set[i] = new char[l];
        infile >> set[i];
    }
    
    printf("... done\n\n");
    printf("CSP parameters:\n");
    printf("  Alphabet length is %li\n", m);
    printf("  Set size is %li\n", n);
    printf("  String length is %li\n", l);
}

/* Destructor */
CSP::~CSP() {
    printf("CSP Destructor");
    
    free(alphabet);
    free(set);
}

/* Return the size of the alphabet A */
long int CSP::getAlphabetSize() {
    return m;
}

/* Return the size of the set S */
long int CSP::getSetSize() {
    return n;
}

/* Return the size of each string */
long int CSP::getStringSize() {
    return l;
}

/* Compute the Hamming distance between two strings */
long int CSP::hamming(char* entry, char* string) {
    long int d = 0;
    for (int i = 0; i < l; i++) {
        if (entry[i] != string[i]) {
            d++;
        }
    }
    return d;
}

/* Compute the max Hamming distance between a given solution and the set S */
long int CSP::getDistance(long int* solution) {
    // Transform solution to alphabet string
    char* string = new char[l];
    for (int i = 0; i < l; i++) {
        long int idx = solution[i];
        string[i] = alphabet[idx];
    }
    
    // Compute the max distance between this string
    // and all strings in the set
    long int max_d = 0;
    for (int i = 0; i < n; i++) {
        char* entry = set[i];
        long int d = hamming(entry, string);
        if (d > max_d) {
            max_d = d;
        }
    }
    return max_d;
}

/* How many strings in the set have the letter i at the position j in the string */
long int CSP::getCount(long int i, long int j) {
    char letter = alphabet[i];
    
    long int count = 0;
    for (int i = 0; i < n; i++) {
        char* string = set[i];
        if (string[j] == letter) {
            count++;
        }
    }
    return count;
}



