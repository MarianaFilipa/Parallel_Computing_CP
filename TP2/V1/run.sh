#!/bin/bash
module load gcc/7.2.0
export CP_CLUSTERS=32

perf stat -e instructions,cycles,L1-dcache-load-misses -M CPI make runpar