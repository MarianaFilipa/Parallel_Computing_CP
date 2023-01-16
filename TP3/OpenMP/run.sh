#!/bin/bash
#SBATCH --time=1:00
#SBATCH --partition=cpar
#SBATCH --cpus-per-task=20
###SBATCH --constraint=k20

module load gcc/7.2.0
export CP_CLUSTERS=32

perf stat -e instructions,cycles,L1-dcache-load-misses -M CPI make runseq
# -n --stdio

#perf record make runpar
#perf report -s pid > perfreport