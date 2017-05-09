//
//  main.cpp
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
#include <vector>

#include "ant.hpp"
#include "csp.hpp"

char* instance_file=NULL;
CSP*  csp;

/* Probabilistic rule related variables */
double** pheromone;      /* pheromone matrix */
double** heuristic;      /* heuristic information matrix */
double** probability;    /* combined value of pheromone X heuristic information */

long int max_budget;     /* The amount of solutions constructed by the ants */
long int budget=0;
double alpha;
double beta;
double rho;
long int n_ants;
long int seed = -1;

std::vector<Ant> colony;
Ant best_ant;
long int best_string_len=LONG_MAX;     /* length of the best string found */

/* Default parameters */
void setDefaultParameters() {
    alpha=1;
    beta=1;
    rho=0.2;
    n_ants=10;
    max_budget=1000;
    instance_file=NULL;
    seed=(long int) time(NULL);
}

/* Print default parameters */
void printParameters() {
    std::cout << "\nACO parameters:\n"
    << "  ants: "  << n_ants << "\n"
    << "  alpha: " << alpha << "\n"
    << "  beta: "  << beta << "\n"
    << "  rho: "   << rho << "\n"
    << "  budget: " << max_budget << "\n"
    << "  seed: "   << seed << "\n"
    << std::endl;
}

/* Print help when program is called with no params */
void printHelp() {
    std::cout << "\nACO Usage:\n"
    << "   ./aco --ants <int> --alpha <float> --beta <float> --rho <float> --budget <int> --seed <int> --instance <path>\n\n"
    << "Example: ./aco --budget 1000 --seed 123 --instance 2-30-10000-1-9.csp\n\n"
    << "\nACO flags:\n"
    << "   --ants: Number of ants to build every iteration. Default=10.\n"
    << "   --alpha: Alpha parameter (float). Default=1.\n"
    << "   --beta: Beta parameter (float). Default=1.\n"
    << "   --rho: Rho parameter (float). Defaut=0.2.\n"
    << "   --budget: Maximum number of strings to build (integer). Default=1000.\n"
    << "   --seed: Number for the random seed generator.\n"
    << "   --instance: Path to the instance file\n"
    << std::endl;
}


/* Read arguments from command line */
bool readArguments(int argc, char* argv[]) {
    
    setDefaultParameters();
    
    for(int i=1; i< argc ; i++){
        if(strcmp(argv[i], "--ants") == 0){
            n_ants = atol(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--alpha") == 0){
            alpha = atof(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--beta") == 0){
            beta = atof(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--rho") == 0) {
            rho = atof(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--budget") == 0) {
            max_budget = atol(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--seed") == 0) {
            seed = atol(argv[i+1]);
            i++;
        } else if(strcmp(argv[i], "--instance") == 0) {
            instance_file = argv[i+1];
            i++;
        } else if(strcmp(argv[i], "--help") == 0) {
            printHelp();
            return(false);
        } else {
            std::cout << "Parameter " << argv[i] << "no recognized.\n";
            return(false);
        }
    }
    if(instance_file==NULL){
        std::cout << "No instance file provided.\n";
        return(false);
    }
    printParameters();
    return(true);
}

void printPheromone () {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    printf("\nPheromone:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < l; j++) {
            printf(" %4.4lf", pheromone[i][j]);
        }
        printf("\n");
    }
}

void printProbability () {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    printf("\nProbability:\n");
    for (int i = 0 ; i < m; i++) {
        for (int j = 0 ; j < l; j++) {
            printf(" %4.4lf ", probability[i][j]);
        }
        printf("\n");
    }
}

/* Create colony structure */
void createColony() {
    std::cout << "Creating colony.\n\n";
    for (int i = 0 ; i < n_ants; i++) {
        colony.push_back(Ant(csp, probability, &seed));
    }
}

/* Initialize pheromone with an initial value */
void initializePheromone() {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    pheromone = new double* [m];
    for (int i = 0 ; i < m; i++) {
        pheromone[i] = new double[l];
        for (int j = 0; j < l; j++) {
            pheromone[i][j] = (double)(1.0 / m);
        }
    }
}

/* Initialize the heuristic information matrix */
void initializeHeuristic () {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    heuristic = new double* [m];
    for (int i = 0 ; i < m; i++) {
        heuristic[i] = new double[l];
        for (int j = 0; j < l; j++) {
            heuristic[i][j] = (double)(1.0 / csp->getCount(i, j));
        }
    }
}

/* Initialize the probability information matrix */
void initializeProbability () {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    probability = new double* [m];
    for (int i = 0; i < m; i++) {
        probability[i] = new double[l];
        for (int j = 0; j < l; j++) {
            probability[i][j] = 0.0;
        }
    }
}

/* Calculate probability using heuristic information and pheromone */
void calculateProbability () {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < l; j++) {
            probability[i][j] =
                pow(pheromone[i][j], alpha) * pow(heuristic[i][j], beta);
        }
    }
}

/* Pheromone evaporation */
void evaporatePheromone(){
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    for (int i = 0 ; i < m; i++) {
        for (int j = 0; j < l; j++) {
            pheromone[i][j] = (double)(1.0 - rho) * pheromone[i][j];
        }
    }
}

/* Pheromone evaporation of best string only */
void evaporatePheromone(Ant best_ant) {
    long int l = csp->getStringSize();
    
    for (int j = 0; j < l; j++) {
        // Get the idx in the alphabet of the j'th letter
        // of the ant's solution string
        long int i = best_ant.getLetter(j);
        pheromone[i][j] = (double)(1.0 - rho) * pheromone[i][j];
    }
}

/* Adds pheromone to the matrix */
void addPheromone(long int i, long int j, double delta) {
    pheromone[i][j] = pheromone[i][j] + delta;
}

/* Update pheromone */
void depositPheromone(){
    long int l = csp->getStringSize();
    double deltaf;
    
    for (int a = 0; a < n_ants; a++) {
        deltaf = (double)(1.0 - (colony[a].getStringDistance() / l));
        for (int j = 0; j < l; j++) {
            // Get the idx in the alphabet of the j'th letter
            // of the ant's solution string
            long int i = colony[a].getLetter(j);
            addPheromone(i, j, deltaf);
        }
    }
}

/* Update pheromone of best string only */
void depositPheromone(Ant best_ant) {
    long int l = csp->getStringSize();
    double deltaf = 1.0 - (double) best_ant.getStringDistance() / l;
    
    for (int j = 0; j < l; j++) {
        long int i = best_ant.getLetter(j);
        addPheromone(i, j, deltaf);
    }
}

/* Check termination condition based on budget */
bool terminationCondition(){
    if (max_budget != 0 && budget >= max_budget)
        return(true);
    return(false);
}

/* Free memory used */
void freeMemory(){
    
    delete csp;
    for(int i = 0; i < csp->getAlphabetSize(); i++){
        delete[] pheromone[i];
        delete[] heuristic[i];
        delete[] probability[i];
    }
    delete[] pheromone;
    delete[] heuristic;
    delete[] probability;
    for (int i = 0; i < n_ants; i++) {
        colony[i].freeAnt();
    }
}

/* MAIN */
int main(int argc, char *argv[] ){
    if(!readArguments(argc, argv)){
        exit(1);
    }
    
    csp = new CSP(instance_file);
    
    initializePheromone();
    initializeHeuristic();
    initializeProbability();
    calculateProbability();
    createColony();
    
    // Iterations loop
    while(!terminationCondition()){

        for(int i = 0; i < n_ants; i++) {
            // Construct solution
            colony[i].Search();
            // Check for new local optimum
            if (best_string_len > colony[i].getStringDistance()) {
                best_string_len = colony[i].getStringDistance();
                best_ant = colony[i];
                printf("%ld:%ld\n", budget, best_string_len);
            }
            budget++;
        }
        
        evaporatePheromone(best_ant);
        depositPheromone(best_ant);
        calculateProbability();
    }
    
    freeMemory();
    std::cout << "\nEnd ACO execution.\n" << std::endl;
    std::cout << "\nBest solution found: " << best_string_len << "\n";
}
