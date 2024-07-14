#!/bin/bash
#SBATCH -N 1
#SBATCH --ntasks 2
#SBATCH -t 3
mpirun ./heat2d --m 2688 --n 4096 --epsilon 0.001 --max-iterations 1000