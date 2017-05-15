#!/usr/bin/env Rscript

# Wilcoxon test

files <- list.files(path="./../output/", pattern = "\\.txt$")
for (i in 1:length(files)) {
    file.name = files[i]
    path.name <- paste("./../output/", file.name, sep="")
    all.data <- read.table(file=path.name, header=TRUE, sep=":") #Replace the separator for any of your preference
    trials <- all.data[,1]  # Remove this line if there are no trial/instances
    data <- all.data[,-1, drop=FALSE]

    source("R-scripts/wilcoxon.R")
    output.name <- paste("./../output/stats/wilcox-", file.name, sep="")
    do.wilcoxon(data.matrix=data, output=output.name)
}
