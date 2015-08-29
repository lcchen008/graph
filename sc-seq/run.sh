#!/bin/bash

module swap mvapich2 impi

#SBATCH -t 00:05:00
#SBATCH -n1 -N1
#SBATCH -p development
#SBATCH --mail-type=end

export OMP_NUM_THREADS=64
export MIC_OMP_NUM_THREADS=840
export MV2_ENABLE_AFFINITY=0
export MIC_PPN=1
export PPN=1
export MIC_LD_LIBRARY_PATH=$MIC_LD_LIBRARY_PATH:/work/02687/binren/boost_1_56_0/stage/lib
export MIC_LD_LIBRARY_PATH=$MIC_LD_LIBRARY_PATH:/opt/apps/intel/13/composer_xe_2013.2.146/compiler/lib/mic/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/work/02687/binren/boost_1_56_0_cpu/stage/lib

ibrun ./a.out 
