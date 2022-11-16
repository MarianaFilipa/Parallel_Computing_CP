#!/bin/bash



perf record  ./bin/k_means 100000000 4 4
perf report -s pid --stdio > perfreport
