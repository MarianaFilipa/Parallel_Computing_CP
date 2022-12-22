#!/bin/bash
module load gcc/7.2.0
module load cuda/11.3.1
export CP_CLUSTERS=4

nvprof ./bin/k_means
