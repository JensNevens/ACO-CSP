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
    printParameters();
}

/* Destructor */
CSP::~CSP() {
    printf("CSP Destructor");
    
    free(alphabet);
    free(set);
}

void CSP::printParameters() {
    std::cout << "\nCSP parameters:\n"
    << "  alphabet length m: "  << m << "\n"
    << "  set size n: " << n << "\n"
    << "  string length l: " << l << "\n"
    << std::endl;
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

/* Transform a solution to an alphabet string */
char* CSP::solution2string(long int* solution) {
    char* string = new char[l];
    long int idx;
    for (int i = 0; i < l; i++) {
        idx = solution[i];
        string[i] = alphabet[idx];
    }
    return string;
}

/* Transform an alphabet string to a solution */
long int* CSP::string2solution(char* string) {
    long int* solution = new long int[l];
    long int idx = 0;
    for (int i = 0; i < l; i++) {
        char letter = string[i];
        for (int j = 0; j < m; j++) {
            if (letter == alphabet[j]) {
                idx = j;
                break;
            }
        }
        solution[i] = idx;
    }
    return solution;
}

/* Compute the max Hamming distance between a given solution and the set S */
long int CSP::getDistance(long int* solution) {    
    // Transform solution to alphabet string
    char* string = solution2string(solution);
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
    // Free up memory
    delete [] string;
    return max_d;
}

/* Compute the Hamming distance between a given solution and the string at index i */
long int CSP::getDistance(long int* solution, long int i) {
    // Transform solution to alphabet string
    char* string = solution2string(solution);
    // Get the correct string
    char* entry = set[i];
    long int d = hamming(entry, string);
    // Free memory
    delete [] string;
    return d;
}

/* Compute the Hamming distance between a given solution and the set S */
long int* CSP::getAllDistances(long int* solution) {
    // Initialize distances array
    long int* distances = new long int[n];
    // Transform solution to alpahbet string
    char* string = solution2string(solution);
    for (int i = 0; i < n; i++) {
        char* entry = set[i];
        long int d = hamming(entry, string);
        distances[i] = d;
    }
    // Free up memory
    delete [] string;
    
    return distances;
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

/* Compare if the given string has the same letter as string j from the set, on position pos */
bool CSP::sameLetters(char* string, long int j, long int pos) {
    char* entry = set[j];
    bool same = entry[pos] == string[pos];
    return same;
}

/* Compare if string i and string j from the set have the same letter at position pos */
bool CSP::sameLetters(long int i, long int j, long int pos) {
    char* entry_i = set[i];
    char* entry_j = set[j];
    bool same = entry_i[pos] == entry_j[pos];
    return same;
}

/* Get the letter of string i at position pos */
char CSP::getLetter(long int i, long pos) {
    char* entry = set[i];
    char letter = entry[pos];
    return letter;
}

