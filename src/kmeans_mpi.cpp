#include "global.h"
#include "string.h"


inline unsigned int maskForMode(unsigned int x,unsigned int y,unsigned int z,unsigned int w ){
    unsigned int max = x > y ? x : y;
    max = z > max ? z : max;
    max = w > max ? w : max;
    unsigned int mask = 0;
    
    if (max == x){
        mask |= 1;
    }
    if (max == y){
        mask |= 2; // 010
    }
    if (max == z){
        mask |= 4; // 0100
    }
    if (max == w){
        mask |= 8; // 0100
    }
    return mask;
}

inline void logDistanceSequence(sequence_t sequence){
    #if DEBUG
        printf("Calculating distance for sequence: ");
        print_sequence(sequence);
        printf("\n\n");
    #endif
}

void kmeans() {
	long delta; //Number of objects has diverged in current iteration
	long nearest; //Nearest centroid
	unsigned int distance,min_distance; //distance calculated by relation point-cluster
    
	unsigned int *tmp_centroidCount = NULL;
	label = (int*)calloc(data_size,sizeof(int));
	centroids = (sequence_t*)calloc(clusters,sizeof(sequence_t));
	tmp_centroidCount = (unsigned int*)malloc(clusters * BIT_SIZE_OF(sequence_t) * sizeof(unsigned int));

    memset (label,-1,data_size * sizeof(int));
    
	for(size_t i = 0;i < clusters;i++) { 
		size_t h = i * data_size / clusters;
		centroids[i] = copy_sequence(data[h]);
	}


	int pc = 0;
	do {

		//Initialize tmp variables

		delta = 0;
		memset (tmp_centroidCount,0,clusters * BIT_SIZE_OF(sequence_t) * sizeof(unsigned int));

		//For each point...
        
		for(size_t i = mpi_rank;i < data_size;i+= mpi_size) {
            
            logDistanceSequence(data[i]);

			min_distance = UINT_MAX;
			nearest = -1;

			for(size_t j = 0;j < clusters;j++) {
                distance = dist_sequence(data[i],centroids[j]);
				if(distance < min_distance) {
					nearest = j;
					min_distance = distance;
				}
                #if DEBUG
                    printf("Distance from cluster %3ld:",j);
                    print_sequence(centroids[j]);
                    printf("\n");
                    printf("Distance =: %d\n\n",distance);
                #endif
			}

			if(label[i] != nearest) {
				delta++;
				label[i] = nearest;
                #if DEBUG
                    printf("Shortest distance is cluster %3ld",nearest);
                    printf("\n");
                    printf("Distance = %d\n\n",min_distance);
                #endif
            }
            #if DEBUG
                printf("label[%u] = %d\n",i,label[i]);
            
            #endif
            unsigned int *tmp_centroid = &tmp_centroidCount[label[i] * BIT_SIZE_OF(sequence_t)];
            for (size_t j=0;j<SEQ_DIM_BITS_SIZE;j++){
            	// bits tmp_centroid[0] is less significative bit from sequence_t
                // bits tmp_centroid[0] = z << 0
                unsigned long int mask = 1;
                if (data[i].z & (mask << j)){
                    tmp_centroid[j]++;
                }
                if (data[i].y & (mask << j)){
                    tmp_centroid[SEQ_DIM_BITS_SIZE + j]++;
                }
                if (data[i].x & (mask << j)){
                    tmp_centroid[(2 *SEQ_DIM_BITS_SIZE) + j]++;
                }
            }
            #if DEBUG
                printf("Added sequence %ld to group %d \n",i,label[i]);
                printf("Group new values are:\n");
                for (j=BIT_SIZE_OF(sequence_t)-1;j>=0; j--) {
                    printf("%d",tmp_centroid[j]);
                }
                printf("\n\n");
            #endif
		}

        #if DEBUG
        for (size_t k=0;k<clusters;k++){
            unsigned int *tmp_centroid = &tmp_centroidCount[k * BIT_SIZE_OF(sequence_t)];
            for (int x=BIT_SIZE_OF(sequence_t)-1;x>=0; x--) {
                printf("%d",tmp_centroid[x]);
            }
			printf("\n");
        }
        #endif

		for(size_t i = 0;i < clusters;i++) {
			sequence_t seq = make_ulong3(0,0,0);
            
            unsigned int *tmp_centroid = &tmp_centroidCount[i* BIT_SIZE_OF(sequence_t)];

            for (size_t j = 0; j < SEQ_DIM_BITS_SIZE; j+= 4) {

            	// bits tmp_centroid[0] is less significative bit from sequence_t
            	// bits tmp_centroid[0] = z << 0
                unsigned int *bitCountX = &tmp_centroid[j + (SEQ_DIM_BITS_SIZE * 2)];
                unsigned int *bitCountY = &tmp_centroid[j + SEQ_DIM_BITS_SIZE];
                unsigned int *bitCountZ = &tmp_centroid[j];
                
                unsigned long int mask = maskForMode(bitCountX[0],bitCountX[1],bitCountX[2],bitCountX[3]);
                seq.x |= (mask << j);
                mask = maskForMode(bitCountY[0],bitCountY[1],bitCountY[2],bitCountY[3]);
                seq.y |= (mask << j);
                mask = maskForMode(bitCountZ[0],bitCountZ[1],bitCountZ[2],bitCountZ[3]);
                seq.z |= (mask << (j));
            }
			centroids[i] = seq;
			#if DEBUG
            	print_sequence(centroids[i]);
            	printf("\n");
			#endif
		}
		printf ("%d - delta = %ld\n",pc,delta);
		pc++;   

	}
    
    while(delta > 0);
#if DEBUG
    for(i = 0;i < clusters;i++) {
       	printf ("cluster count = %d\n",count[i]);
    }
#endif
}
