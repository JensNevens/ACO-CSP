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
CSP* csp;

/*Probabilistic rule related variables*/
double  ** pheromone;      /* pheromone matrix */
double  ** heuristic;      /* heuristic information matrix */
double  ** probability;    /* combined value of pheromone X heuristic information */
double initial_pheromone=1.0;

long int max_budget;       // The amount of solutions constructed by each ant
long int budget=0;
double alpha;
double beta;
double rho;
long int n_ants;
long int seed = -1;

std::vector<Ant> colony;
Ant best_ant;
long int best_tour_length=LONG_MAX;     /* length of the shortest tour found */

/*Default parameters*/
void setDefaultParameters() {
    alpha=1;
    beta=1;
    rho=0.2;
    n_ants=10;
    max_budget=1000;
    instance_file=NULL;
    seed=seed= (long int) time(NULL);
}

/*Print default parameters*/
void printParameters() {
    std::cout << "\nACO parameters:\n"
    << "  ants: "  << n_ants << "\n"
    << "  alpha: " << alpha << "\n"
    << "  beta: "  << beta << "\n"
    << "  rho: "   << rho << "\n"
    << "  budget: " << max_budget << "\n"
    << "  seed: "   << seed << "\n"
    << "  initial pheromone: "   << initial_pheromone << "\n"
    << std::endl;
}

/*Print help when program is called with no params*/
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
    << "\nACO other parameters:\n"
    << "   initial pheromone: "   << initial_pheromone << "\n"
    << std::endl;
    
}


/* Read arguments from command line */
bool readArguments(int argc, char *argv[]) {
    
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
    long int i, j;
    long int size = tsp->getSize();
    
    printf("\n");
    for ( i = 0 ; i < size ; i++ ) {
        for ( j = 0 ; j < size ; j++ ) {
            printf(" %lf ", pheromone[i][j]);
        }
        printf("\n");
    }
}

void printProbability () {
    long int i, j;
    long int size = tsp->getSize();
    
    printf("\nProbability:\n");
    for ( i = 0 ; i < size ; i++ ) {
        for ( j = 0 ; j < size ; j++ ) {
            printf(" %4.4lf ", probability[i][j]);
        }
        printf("\n");
    }
}

/*Create colony structure*/
void createColony() {
    std::cout << "Creating colony.\n\n";
    for (int i = 0 ; i < n_ants ; i++ ) {
        colony.push_back(Ant(csp, probability, &seed));
    }
}

/*Initialize pheromone with an initial value*/
void initializePheromone( double initial_value ) {
    long int size = tsp->getSize();
    
    pheromone = new double* [size];
    for (int i = 0 ; i < size ; i++ ) {
        pheromone[i] = new double [size];
        for (int j = 0  ; j < size ; j++ ) {
            if (i==j) pheromone[i][j] = 0.0;
            else pheromone[i][j] = initial_value;
            /* symmetric TSP instances; hence phermone[i][j] = pheromone[j][i] */
        }
    }
}

// Initialize the heuristic information matrix
void initializeHeuristic () {
    long int size = tsp->getSize();
    
    heuristic = new double* [size];
    for (int i = 0 ; i < size ; i++) {
        heuristic[i] = new double[size];
        for (int j = 0 ; j < size ; j++) {
            if (i != j) heuristic[i][j] = 1.0 / tsp->getDistance(i, j);
            else heuristic[i][j] = 0.0;
        }
    }
}

// Initialize the probability information matrix
void initializeProbability () {
    long int size = tsp->getSize();
    
    probability = new double* [size];
    for (int i = 0; i < size; i++) {
        probability[i] = new double[size];
        for (int j = 0; j < size; j++) {
            probability[i][j] = 0.0;
        }
    }
}

// Calculate probability in base of heuristic
// information and pheromone
void calculateProbability () {
    long int size = tsp->getSize();
    
    for (int i = 0 ; i < size ; i++) {
        probability[i][i] = 0.0;
        for (int j = (i+1) ; j < size ; j++) {
            probability[i][j] =
            pow(pheromone[i][j], alpha) * pow(heuristic[i][j], beta);
            probability[j][i] = probability[i][j];
        }
    }
}

/*Pheromone evaporation*/
void evaporatePheromone(){
    long int size = tsp->getSize();
    
    for (int i = 0 ; i < size ; i++ ) {
        for (int j = i+1  ; j < size ; j++ ) {
            pheromone[i][j] = (double)(1.0 - rho) * pheromone[i][j];
            pheromone[j][i] = pheromone[i][j];
        }
    }
}

/*Pheromone evaporation of best tour only*/
void evaporatePheromone(Ant best_ant) {
    long int size = tsp->getSize();
    long int* tour = best_ant.getTour();
    
    for (int i = 0; i < (size-1); i++) {
        long int from = tour[i];
        long int to = tour[i+1];
        pheromone[from][to] = (double)(1.0 - rho) * pheromone[from][to];
        pheromone[to][from] = pheromone[from][to];
    }
    long int last = tour[size-1];
    long int first = tour[0];
    pheromone[last][first] = (double)(1.0 - rho) * pheromone[last][first];
    pheromone[first][last] = pheromone[last][first];
}

/*Adds simetrically pheromone to the matrix*/
void addPheromone(long int i , long int j, double delta) {
    pheromone[i][j] = pheromone[i][j] + delta;
    pheromone[j][i] = pheromone[j][i] + delta;
}

/*Update pheromone*/
void depositPheromone(){
    long int size = tsp->getSize();
    double deltaf;
    
    for (int i=0; i< n_ants; i++) {
        deltaf = 1.0 / (double) colony[i].getTourLength();
        for (int j =1 ; j < size ; j ++) {
            addPheromone(colony[i].getCity(j-1), colony[i].getCity(j), deltaf);
        }
        addPheromone(colony[i].getCity(size-1), colony[i].getCity(0), deltaf);
    }
}

/*Update pheromone of best tour only*/
void depositPheromone(Ant best_ant) {
    long int size = tsp->getSize();
    double deltaf = 1.0 / (double) best_ant.getTourLength();
    
    for (int i=1; i < size; i++) {
        addPheromone(best_ant.getCity(i-1), best_ant.getCity(i), deltaf);
    }
    addPheromone(best_ant.getCity(size-1), best_ant.getCity(0), deltaf);
}

/*Check termination condition based on iterations or tours.
 One of the criteria must be active (=!0).*/
bool terminationCondition(){
    if (max_tours != 0 && tours >= max_tours)
        return(true);
    if (max_iterations !=0 && iterations >= max_iterations)
        return(true);
    return(false);
}

/*Free memory used*/
void freeMemory(){
    
    delete tsp;
    for(int i=0; i < tsp->getSize(); i++){
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

/*This function calls methods that MUST be implemented by you*/
int main(int argc, char *argv[] ){
    if(!readArguments(argc, argv)){
        exit(1);
    }
    
    tsp = new TSP(instance_file, verbose);
    
    initializePheromone(initial_pheromone);
    initializeHeuristic();
    initializeProbability();
    calculateProbability();
    createColony();
    
    //Iterations loop
    while(!terminationCondition()){
        //build solutions
        for(int i=0; i< n_ants; i++){
            //Construct solution
            colony[i].Search();
            colony[i].localPheromoneUpdate(pheromone, rho, initial_pheromone);
            /*Check for new local optimum*/
            if(best_tour_length > colony[i].getTourLength()){
                best_tour_length = colony[i].getTourLength();
                best_ant = colony[i];
                printf("%ld:%ld\n", tours, best_tour_length);
                if (verbose) {
                    std::cout << "\nNew best solution found! Cost: "
                    << best_tour_length
                    << "\n";
                }
            }
            tours++;
        }
        
        evaporatePheromone(best_ant);
        depositPheromone(best_ant);
        calculateProbability();
        iterations++;
    }
    
    freeMemory();   // Free memory.
    if (verbose) {
        std::cout << "\nEnd ACO execution.\n" << std::endl;
        std::cout << "\nBest solution found: " << best_tour_length << "\n";
    }
    printf("Best %ld\n", best_tour_length);
}
