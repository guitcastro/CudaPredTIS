#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "io.h"

extern void kmeans(void);

sequence_t *data;
size_t  data_size;
size_t clusters;
sequence_t *centroids;
int *label;

int mpi_rank;
int mpi_size;

int main(int argc,char **argv) {	

#if USE_MPI
	int provider;
    MPI_Init_thread(&argc,&argv,MPI_THREAD_FUNNELED,&provider);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);
#endif

	char filename[255];
	if(argc < 3) {
		printf("Invalid arguments, usage:\n");
		printf("\tpdtis [INPUT] [CLUSTERS]\n");
		return 0;
	}
	
	//Execute processing
	clusters = atoi(argv[2]);
	read_data(argv[1]);
	kmeans();
	const char * outName = (argc > 3) ? argv[3] : argv[1];
	sprintf(filename,"%s.out",outName);
	write_nearest_objects(filename);

	return 0;	
}
