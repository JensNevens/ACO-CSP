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

#include "utils.hpp"
#include "ant.hpp"
#include "csp.hpp"


char* instance_file=NULL;
CSP*  csp;

/* Probabilistic rule related variables */
double** pheromone;      /* pheromone matrix */
double** heuristic;      /* heuristic information matrix */
double** probability;    /* combined value of pheromone X heuristic information */
double initial_pheromone;

bool as;                 /* Flag to indicate whether to use Ant System */
long int max_budget;     /* The max amount of solutions constructed by the ants */
long int budget=0;       /* The current amount of solutions constructed */
double alpha;
double beta;
double rho;
long int n_ants;
long int seed = -1;

bool mmas;               /* Flag to indicate whether to use Min Max Ant System */
double tau_max;
double tau_min;

bool local;              /* Flag to indicate whether to use local search */
double b_rep;            /* Local search parameter */

bool acs;                /* Flag to indicate whether to use Ant Colony System */
double q0;

bool verbose;            /* Flag to indicate whether to print lots of info */

std::vector<Ant> colony;
Ant best_ant;
long int best_string_len=LONG_MAX;     /* length of the best string found */

/* Default parameters */
void setDefaultParameters() {
    as=false;
    mmas=false;
    acs=false;
    local=false;
    verbose=false;
    
    alpha=1;
    beta=1;
    rho=0.1;
    n_ants=10;
    max_budget=1000;
    instance_file=NULL;
    seed=(long int) time(NULL);
    b_rep=0.001;
    q0=0.9;
}

/* Print default parameters */
void printParameters() {
    std::cout << "\nACO parameters:\n"
    << "  ants: "   << n_ants << "\n"
    << "  alpha: "  << alpha << "\n"
    << "  beta: "   << beta << "\n"
    << "  rho: "    << rho << "\n"
    << "  budget: " << max_budget << "\n"
    << "  seed: "   << seed << "\n"
    << "  local: "  << local << "\n"
    << "  b_rep: "  << b_rep << "\n"
    << "  q0: "     << q0 << "\n"
    << "  verbose: " << verbose << "\n"
    << std::endl;
}

/* Print help when program is called with no params */
void printHelp() {
    std::cout << "\nACO Usage:\n"
    << "   ./aco --ants <int> --alpha <float> --beta <float> --rho <float> --budget <int> --seed <int> --instance <path>\n\n"
    << "Example: ./aco --budget 1000 --seed 123 --instance 2-30-10000-1-9.csp\n\n"
    << "\nACO flags:\n"
    << "   --as: Flag to inidcate Ant System usage\n"
    << "   --mmas: Flag to indicate Min Max Ant System usage\n"
    << "   --acs: Flag to indicate Ant Colony System usage\n"
    << "   --local: Flag to indicate Local Search usage\n"
    << "   --ants: Number of ants to build every iteration. Default=10.\n"
    << "   --alpha: Alpha parameter (float). Default=1.\n"
    << "   --beta: Beta parameter (float). Default=1.\n"
    << "   --rho: Rho parameter (float). Defaut=0.1.\n"
    << "   --budget: Maximum number of strings to build (integer). Default=1000.\n"
    << "   --seed: Number for the random seed generator.\n"
    << "   --instance: Path to the instance file\n"
    << "   --brep: Control number of Local Search steps. Default=0.001.\n"
    << "   --q0: Probability for exploration in Ant Colony System. Default=0.9.\n"
    << "   --v: Flag to indicate verbosity of the output.\n"
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
        } else if(strcmp(argv[i], "--mmas") == 0) {
            mmas = true;
        } else if(strcmp(argv[i], "--local") == 0) {
            local = true;
        } else if (strcmp(argv[i], "--brep") == 0) {
            b_rep = atof(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--acs") == 0) {
            acs = true;
        } else if (strcmp(argv[i], "--q0") == 0) {
            q0 = atof(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--as") == 0) {
            as = true;
        } else if (strcmp(argv[i], "--v") == 0) {
            verbose = true;
        } else if(strcmp(argv[i], "--help") == 0) {
            printHelp();
            return(false);
        } else {
            std::cout << "Parameter " << argv[i] << "no recognized.\n";
            return(false);
        }
    }
    if (instance_file==NULL) {
        std::cout << "No instance file provided.\n";
        return(false);
    }
    if ((mmas && acs) || (mmas && as) || (acs && as)) {
        std::cout << "Cannot use multiple algorithms at the same time!\n";
        return(false);
    }
    if (verbose)
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
    if (verbose)
        std::cout << "Creating colony.\n\n";
    for (int i = 0 ; i < n_ants; i++) {
        if (acs) {
            colony.push_back(Ant(csp, probability, &seed, q0));
        } else {
            colony.push_back(Ant(csp, probability, &seed));
        }
        
    }
}

/* Initialize parameters of the ACO algorithm */
void initializeParameters() {
    initial_pheromone = 1.0 / (double) csp->getAlphabetSize();
    if (mmas) {
        tau_max = 1.0 / (double) csp->getAlphabetSize();
        double ratio = (double) csp->getAlphabetSize() * (double) csp->getStringSize();
        tau_min = tau_max / ratio;
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
            if (mmas) {
                pheromone[i][j] = tau_max;
            } else {
                pheromone[i][j] = initial_pheromone;
            }
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
            heuristic[i][j] =
                (double) csp->getCount(i, j) / (double) csp->getSetSize();
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

/* Bound pheromone to range [tau_min, tau_max] */
void boundPheromone(long int i, long int j) {
    if (pheromone[i][j] > tau_max) {
        pheromone[i][j] = tau_max;
    } else if (pheromone[i][j] < tau_min) {
        pheromone[i][j] = tau_min;
    }
}

/* Pheromone evaporation */
void evaporatePheromone() {
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < l; j++) {
            pheromone[i][j] = (double) (1.0 - rho) * pheromone[i][j];
            if (mmas)
                boundPheromone(i, j);
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
        pheromone[i][j] = (double) (1.0 - rho) * pheromone[i][j];
        
        if (mmas) {
            boundPheromone(i, j);
        }
    }
}

/* Adds pheromone to the matrix */
void addPheromone(long int i, long int j, double delta) {
    pheromone[i][j] = pheromone[i][j] + delta;
    
    if (mmas) {
        boundPheromone(i, j);
    }
}

/* Update pheromone */
void depositPheromone(){
    long int l = csp->getStringSize();
    double deltaf;
    
    for (int a = 0; a < n_ants; a++) {
        deltaf = 1.0 - ((double) colony[a].getStringDistance() / (double) l);
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
    long int m = csp->getAlphabetSize();
    long int l = csp->getStringSize();
    double deltaf;
    
    if (mmas) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < l; j++) {
                long int letter = best_ant.getLetter(j);
                if (letter == i) {
                    deltaf = rho * tau_max;
                } else {
                    deltaf = rho * tau_min;
                }
                addPheromone(i, j, deltaf);
            }
        }
    } else {
        deltaf = 1.0 - ((double) best_ant.getStringDistance() / (double) l);
        for (int j = 0; j < l; j++) {
            long int i = best_ant.getLetter(j);
            addPheromone(i, j, deltaf);
        }
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
    
    csp = new CSP(instance_file, verbose);
    
    initializeParameters();
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
            // If Ant Colony System, do local pheromone update
            if (acs)
                colony[i].LocalPheromoneUpdate(pheromone, rho, initial_pheromone);
            // If local search, do local search
            if (local)
                colony[i].LocalSearch(b_rep);
            // Check for new local optimum
            if (best_string_len > colony[i].getStringDistance()) {
                best_string_len = colony[i].getStringDistance();
                best_ant = colony[i];
                printf("%ld:%ld\n", budget, best_string_len);
            }
            budget++;
        }
        // Update pheromones and probabilities
        evaporatePheromone();
        if (mmas || acs) {
            // Only the best ant deposits pheromone
            depositPheromone(best_ant);
        } else if (as) {
            // All ants deposit pheromone
            depositPheromone();
        }
        calculateProbability();
    }
    // Free memory
    freeMemory();
    if (verbose) {
        std::cout << "\nEnd ACO execution.\n" << std::endl;
        std::cout << "\nBest solution found: " << best_string_len << "\n";
    }
    std::cout << best_string_len << "\n";
}
