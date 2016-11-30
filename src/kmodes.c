#include "global.h"
#include "kmodes.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

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

kmodes_result_t kmodes(kmodes_input_t input) {
  printf("Execution sequential Kmeans\n");
  long delta; //Number of objects has diverged in current iteration
  long nearest; //Nearest centroid
  size_t distance,min_distance; //distance calculated by relation point-cluster
  size_t clusters = input.number_of_clusters;
  size_t data_size = input.data_size;
  sequence_t *data = input.data;
  int* label = (int*)calloc(data_size, sizeof(int));
  sequence_t *centroids = (sequence_t*)calloc(clusters, sizeof(sequence_t));
  unsigned int *tmp_centroidCount = (unsigned int*)malloc(clusters * BIT_SIZE_OF(sequence_t) * sizeof(unsigned int));

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

    for(size_t i = 0;i < data_size;i++) {

      min_distance = UINT_MAX;
      nearest = -1;

      for(size_t j = 0;j < clusters;j++) {
        distance = dist_sequence(data[i],centroids[j]);
        if(distance < min_distance) {
          nearest = j;
          min_distance = distance;
        }
      }

      if(label[i] != nearest) {
        delta++;
        label[i] = nearest;
      }

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

    }

    for(size_t i = 0;i < clusters;i++) {
      sequence_t seq = { 0,0,0 };

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

    }
    printf ("%d - delta = %ld\n",pc,delta);
    pc++;

  }
  while(delta > 0);

  kmodes_result_t result = {
    label,
    centroids
  };
  return result;
}
