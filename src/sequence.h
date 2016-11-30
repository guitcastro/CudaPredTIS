//
//  sequence.h
//  TD
//
//  Created by Guilherme Torres on 9/7/13.
//  Copyright (c) 2013 Guilherme Torres. All rights reserved.
//

#ifndef TD_sequence_h
#define TD_sequence_h

#include <stdint.h>

#ifdef __INTEL_COMPILER
  #define __OFFLOAD__MODIFIER__ __attribute__((target(mic)))
#elif __CUDACC__
  #define __host__ __device__
#else
  #define __OFFLOAD__MODIFIER__
#endif

typedef struct ulong3
{
  unsigned long int x, y, z;
} sequence_t;

#define SEQ_DIM_BITS_SIZE sizeof(unsigned long int) * 8

__OFFLOAD__MODIFIER__ void print_sequence(sequence_t seq);

__OFFLOAD__MODIFIER__ int dist_sequence(sequence_t seq1,sequence_t seq2);

__OFFLOAD__MODIFIER__ sequence_t copy_sequence(sequence_t seq);

__OFFLOAD__MODIFIER__ sequence_t sum_sequence(sequence_t seq1,sequence_t seq2);

__OFFLOAD__MODIFIER__ sequence_t div_sequence(sequence_t seq1, unsigned long div);


#endif
