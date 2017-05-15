#!/bin/bash

# USAGE: ./run-algorithms.sh instance-dir folder

# inputs
instancedir=$1
folder=$2

for instance in ${instancedir}/*.csp; do
  filename=$(basename "$instance")
  extension="${filename##*.}"
  filename="${filename%.*}"

  echo "trial:as:mmas:acs" > ${folder}/csp${filename}.txt

  # Set initial seed
  seed=660
  for i in {1..10}; do
    AUX="${i}" # number of trial
    for a in as mmas acs; do
      # Write header to file
      echo "budget:quality" > ${folder}/convergence/csp${filename}/out-${a}-s${seed}.txt

      # Execute the algorithm with seed and parameters and save the output file in a folder
      if [ $a == as ]
      then
      ./ACO-CSP --as --instance ${instance} --budget 1000 --seed ${seed} --ants 5 --alpha 0.80 --beta 8.37 --rho 0.11 >> ${folder}/convergence/csp${filename}/out-${a}-s${seed}.txt
      elif [ $a == mmas ]
      then
      ./ACO-CSP --mmas --instance ${instance} --budget 1000 --seed ${seed} --ants 50 --alpha 1.10 --beta 9.84 --rho 0.38 >> ${folder}/convergence/csp${filename}/out-${a}-s${seed}.txt
      elif [ $a == acs ]
      then
      ./ACO-CSP --acs --instance ${instance} --budget 1000 --seed ${seed} --ants 50 --alpha 2.2 --beta 9.88 --rho 0.25 --q0 0.24 >> ${folder}/convergence/csp${filename}/out-${a}-s${seed}.txt
      fi

      # Read the output file and extract the best string found
      SOL=$(tail -n 1 ${folder}/convergence/csp${filename}/out-${a}-s${seed}.txt | grep -e '^[[:space:]]*[+-]\?[0-9]' | cut -f1)

      # Save for printing
      AUX="${AUX}:${SOL}"
    done
    # Print matrix of best string found
    echo "$AUX" >> ${folder}/csp${filename}.txt
    # Next seed
    seed=$(( seed + 1 ))
  done
done
