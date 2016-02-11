//
//  sequence.h
//  TD
//
//  Created by Guilherme Torres on 9/7/13.
//  Copyright (c) 2013 Guilherme Torres. All rights reserved.
//

#ifndef TD_sequence_h
#define TD_sequence_h

#include <cuda_runtime.h>

typedef ulong3 sequence_t;

#define SEQ_DIM_BITS_SIZE sizeof(unsigned long int) * 8
__host__ int  dist_sequence(sequence_t seq1,sequence_t seq2);
__host__ __device__ sequence_t  copy_sequence(sequence_t seq);
__host__ __device__ void print_sequence(sequence_t seq);

sequence_t  sum_sequence(sequence_t seq1,sequence_t seq2);
sequence_t div_sequence(sequence_t seq1, unsigned long div);

#endif
