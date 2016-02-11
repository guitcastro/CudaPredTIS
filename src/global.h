#ifndef _GLOBAL_
#define _GLOBAL_


#include <stdio.h>
#include <stdlib.h>
#include "sequence.h"

#define BIT_SIZE_OF(x) (sizeof(x) * 8)

#define DEBUG 0
#define BINARY_OUT 0

extern sequence_t *data;
extern size_t  data_size;
extern size_t clusters;
extern sequence_t *centroids;
extern int *label;

extern int mpi_rank;
extern int mpi_size;

#endif
