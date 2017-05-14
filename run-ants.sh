#!/bin/bash

#USAGE: ./run-ants.sh instance folder

#inputs
instance=$1
folder=$2

rm -rf ${folder}/data-ants.txt

echo "trial:m5:m10:m20:m50:m100" >> ${folder}/data-ants.txt

#Set initial seed for different trials
seed=1234
for i in {1..20}; do
  AUX="${i}" #number of trial
  for m in {5,10,20,50,100}; do
    #Write header to file
    echo "budget:quality" > ${folder}/convergence-ants/output-m${m}-${seed}.txt

    #Execute the algorithm with the seed and the parameter and save the output file in a folder.
    ./ACO-CSP --instance ${instance} --budget 1000 --ants ${m} --seed ${seed}  >>  ${folder}/convergence-ants/output-m${m}-${seed}.txt

    #Read the output file and extract the best string found
    SOL=$(tail -n 1 ${folder}/convergence-ants/output-m${m}-${seed}.txt | grep -e '^[[:space:]]*[+-]\?[0-9]' | cut -f1)
    
    #Save for printing
    AUX="${AUX}:${SOL}"
  done
  #Print matrix of best tour found
  echo "$AUX" >>  ${folder}/data-ants.txt
  #Next seed
  seed=$(( seed + 1 ))
done

#Remove any unuseful file.....
rm -rf stat* cmp* best*

