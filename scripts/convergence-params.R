#!/usr/bin/env Rscript

#
# Example algorithms
#
# Change for the your filenames and the names of your tests
file.names <- c("./../output/convergence/output-as-1234.txt",
                "./../output/convergence/output-mmas-1234.txt",
                "./../output/convergence/output-acs-1234.txt")
test.names <- c("as", "mmas", "acs")

# Read data in a list with "names" as elements
data <- list()
for(i in 1:length(file.names)){
  data[[test.names[i]]] <- read.table(file=file.names[i], header=TRUE, sep=":", nrow=length(readLines(file.names[i])) - 2)
}

source("R-scripts/convergence.R")
do.lines.plot(data, plot.title="Converge for algorithms", output="convergence-algos.png")

#
# Example ants
#
# Change for the your filenames and the names of your tests
# file.names <- c("./../out/convergence-ants/output-m5-1234.txt",
#                "./../out/convergence-ants/output-m10-1234.txt",
#                "./../out/convergence-ants/output-m20-1234.txt",
#                "./../out/convergence-ants/output-m50-1234.txt",
#                "./../out/convergence-ants/output-m100-1234.txt")
#test.names <- c("m5", "m10", "m20", "m50", "m100")

# Read data in a list with "names" as elements
#data <- list()
#for(i in 1:length(file.names)){
#    data[[test.names[i]]] <- read.table(file=file.names[i], header=TRUE, sep=":", nrow=length(readLines(file.names[i])) - 2)
#}


#source("R-scripts/convergence.R")
#do.lines.plot(data, plot.title="Converge for #ants", output="convergence-ants.png")
