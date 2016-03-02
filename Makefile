
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)		
    endif
    ifeq ($(UNAME_S),Darwin)
    	COMPILE_FLAGS=-ccbin=clang-omp++ -Xcompiler -fopenmp
    	LINK_FLAGS=-ccbin=clang-omp++ -Xcompiler -fopenmp
endif

CUDA_HOME?=/Developer/NVIDIA/CUDA-7.5
LDFLAGS=-I${CUDA_HOME}/include
COMPILE_FLAGS+=--ptxas-options=-v -c -arch=sm_20 -Xcompiler -Wall -o 
LINK_FLAGS+=-Xcompiler -fopenmp -o  
MPI_FLAGS=-I$(shell mpicc --showme:incdirs) $(addprefix -L,$(shell mpicc --showme:libdirs)) -Xcompiler -fopenmp

CUDAC=${CUDA_HOME}/bin/nvcc
CC=g++ 

BASES = athaliana celegans Rattusnovergicus Musmusculus HomoSapiens gallus Drosophila
INPUTS = input input_cuda input_mpi
BASE_OBJC = objc/sequence.o objc/io.o
OBJC =  objc/main.o ${BASE_OBJC}

all: create_objc_dir kmeans_cuda sequence io main kmeans kmeans-mpi
	${CUDAC} ${LINK_FLAGS} bin/kmodes objc/kmeans.o ${OBJC} 
	${CUDAC} ${LINK_FLAGS}  bin/cuda-kmodes  objc/kmeans_cuda.o ${OBJC}  
	${CUDAC} $(MPI_FLAGS) -lmpi -lmpi_cxx ${LINK_FLAGS} bin/mpi-kmodes  objc/kmeans_mpi.o objc/main_mpi.o ${BASE_OBJC}  	
	
kmeans_cuda:
	${CUDAC} ${COMPILE_FLAGS} objc/$@.o  src/$@.cu --shared
kmeans:
	${CUDAC} ${COMPILE_FLAGS} objc/$@.o src/$@.cpp
kmeans-mpi:
	${CUDAC} $(MPI_FLAGS) -lmpi -lmpi_cxx  ${COMPILE_FLAGS} objc/kmeans_mpi.o src/kmeans_mpi.cpp -D USE_MPI
	${CUDAC} $(MPI_FLAGS) -lmpi -lmpi_cxx  ${COMPILE_FLAGS} objc/main_mpi.o src/main.cpp -D USE_MPI
sequence:
	${CUDAC} ${COMPILE_FLAGS} objc/$@.o src/$@.cu 
io:
	${CUDAC} ${COMPILE_FLAGS} objc/$@.o src/$@.cpp
main:
	${CUDAC} ${COMPILE_FLAGS} objc/$@.o src/$@.cpp
create_objc_dir:
	mkdir -p objc
extract_input:
	for input in $(INPUTS) ; do \
		for base in $(BASES) ; do \
			COMMAND="tar -zxkvf $$input/$$base/clusters.tar.gz -C $$input/$$base/" ; \
			echo $$COMMAND ; \
			$$COMMAND 2>/dev/null; true ;\
		done ; \
	done	
    
clean:
	for input in $(INPUTS) ; do \
		for base in $(BASES) ; do \
			num=1 ; while [[ $$num -le 10 ]] ; do \
				rm -f $$input/$$base/cluster$$num.out; \
				rm -f $$input/$$base/train$$num; \
				rm -f $$input/$$base/testPos$$num; \
				rm -f $$input/$$base/testNeg$$num; \
				rm -f $$input/$$base/test$$num; \
				rm -f $$input/$$base/results$$num; \
				rm -f $$input/$$base/predictions$$num; \
				rm -f $$input/$$base/model$$num; \
				rm -f $$input/$$base/log$$num.txt; \
				rm -f $$input/$$base/cluster$$num.out; \
				rm -f $$input/$$base/cluster$$num ; \
				rm -f $$input/$$base/cluster$$num.out.binseq; \
				((num = num + 1)) ; \
			done ; \
				rm -f $$input/$$base/resultsFinal; \
				rm -f $$input/$$base/positivos.binseq; \
				rm -f $$input/$$base/negativos.binseq; \
				rm -f $$input/$$base/negativos; \
				rm -f $$input/$$base/positivos; \
		done ; \
	done
	rm -f ${OBJC} objc/kmeans.o objc/kmeans_cuda.o bin/cuda-kmodes bin/kmodes bin/kmodes bin/mpi-kmodes
		
		
	
