module swap mvapich2 impi
export MIC_LD_LIBRARY_PATH=$MIC_LD_LIBRARY_PATH:/work/02687/binren/boost_1_56_0/stage/lib
export MIC_LD_LIBRARY_PATH=$MIC_LD_LIBRARY_PATH:/opt/apps/intel/13/composer_xe_2013.2.146/compiler/lib/mic/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/work/02687/binren/boost_1_56_0_cpu/stage/lib
make -f makefile.mic clean
make -f makefile.mic
make -f makefile.cpu clean
make -f makefile.cpu

