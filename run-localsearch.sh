#!/bin/bash

# USAGE: ./run-localsearch.sh instance-dir folder

# inputs
instancedir=$1
folder=$2

for instance in ${instancedir}/*.csp; do
  filename=$(basename "$instance")
  extension="${filename##*.}"
  filename="${filename%.*}"

  echo "trial:mmas:mmas+ls:mmas+ls+" > ${folder}/localsearch/${filename}.txt

  # Set initial seed
  seed=660
  for i in {1..10}; do
    AUX="${i}" # number of trial
    for a in mmas mmasls mmaslstun; do
      # Write header to file
      echo "budget:quality" > ${folder}/convergence/localsearch/${filename}/out-${a}-s${seed}.txt

      # Execute the algorithm with seed and parameters and save the output file in a folder
      if [ $a == mmas ]
      then
      ./ACO-CSP --mmas --instance ${instance} --budget 1000 --seed ${seed} --ants 50 --alpha 1.10 --beta 9.84 --rho 0.38 >> ${folder}/convergence/localsearch/${filename}/out-${a}-s${seed}.txt
      elif [ $a == mmasls ]
      then
      ./ACO-CSP --mmas --local --brep 0.001 --instance ${instance} --budget 1000 --seed ${seed} --ants 50 --alpha 1.10 --beta 9.84 --rho 0.38 >> ${folder}/convergence/localsearch/${filename}/out-${a}-s${seed}.txt
      elif [ $a == mmaslstun ]
      then
      ./ACO-CSP --mmas --local --brep 0.001 --instance ${instance} --budget 1000 --seed ${seed} --ants 5 --alpha 1.30 --beta 7.70 --rho 0.20 >> ${folder}/convergence/localsearch/${filename}/out-${a}-s${seed}.txt
      fi

      # Read the output file and extract the best string found
      SOL=$(tail -n 1 ${folder}/convergence/localsearch/${filename}/out-${a}-s${seed}.txt | grep -e '^[[:space:]]*[+-]\?[0-9]' | cut -f1)

      # Save for printing
      AUX="${AUX}:${SOL}"
    done
    # Print matrix of best string found
    echo "$AUX" >> ${folder}/localsearch/${filename}.txt
    # Next seed
    seed=$(( seed + 1 ))
  done
done
