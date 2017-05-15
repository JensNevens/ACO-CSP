#!/usr/bin/env Rscript

# Wilcoxon test

args <- commandArgs(trailingOnly=TRUE)
mode <- args[2]

base.path <- ""
if (grepl(mode, "-a")) {
    base.path <- "./../output/algorithms/"
} else if (grepl(mode, "-ls")) {
    base.path <- "./../output/localsearch/"
}

files <- list.files(path=base.path, pattern = "\\.txt$")
for (i in 1:length(files)) {
    file.name = files[i]
    path.name <- paste(base.path, file.name, sep="")
    all.data <- read.table(file=path.name, header=TRUE, sep=":") #Replace the separator for any of your preference
    trials <- all.data[,1]  # Remove this line if there are no trial/instances
    data <- all.data[,-1, drop=FALSE]

    source("R-scripts/wilcoxon.R")
    output.name <- ""
    if (grepl(mode, "-a")) {
        output.name <- paste("./../output/stats/algorithms/wilcox-", file.name, sep="")
    } else if (grepl(mode, "-ls")) {
        output.name <- paste("./../output/stats/localsearch/wilcox-", file.name, sep="")
    }
    do.wilcoxon(data.matrix=data, output=output.name)
}
