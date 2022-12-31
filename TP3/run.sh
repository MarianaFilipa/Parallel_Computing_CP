#!/bin/bash
#SBATCH --time=1:00
#SBATCH --partition=cpar
#SBATCH --constraint=k20

module load gcc/7.2.0
module load cuda/11.3.1
export CP_CLUSTERS=4

nvprof ./bin/k_means 10000000 4 10000