#include "global.h"
#include "string.h"
#include "power.h"
#include "limits.h"

inline unsigned int __attribute__((target(mic))) maskForMode(unsigned int x,unsigned int y,unsigned int z,unsigned int w){
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

void kmeans() {
  power_init();
  #pragma offload target(mic) in(data:length(data_size)) out(label:length(data_size)) inout(centroids:length(clusters))
  {
    printf("Execution sequential Kmeans\n");
    long delta; //Number of objects has diverged in current iteration

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
    int nearests[data_size];
    unsigned int min_distances[data_size];

    do {
      delta = 0;
      memset (tmp_centroidCount,0,clusters * BIT_SIZE_OF(sequence_t) * sizeof(unsigned int));

      #pragma omp parallel for
      for(size_t i = 0;i < data_size;i++) {
        // printf("pc = %d, i = %d\n", pc, i);
        min_distances[i] = UINT_MAX;
        nearests[i] = -1;

        for(size_t j = 0;j < clusters;j++) {
          unsigned int distance = dist_sequence(data[i],centroids[j]);
          // printf("Distance = %d\n", distance);
          if(distance < min_distances[i]) {
            nearests[i] = j;
            min_distances[i] = distance;
          }
        }

        if(label[i] != nearests[i]) {
          #pragma omp atomic
          delta++;
          label[i] = nearests[i];
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

      #pragma omp parallel for
      for(size_t i = 0;i < clusters;i++) {
        sequence_t seq = sequence_t { 0,0,0 };

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

    } while(delta > 0);
  }
  power_end();

}
