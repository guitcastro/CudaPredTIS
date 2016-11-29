//
//  sequence.h
//  TD
//
//  Created by Guilherme Torres on 9/7/13.
//  Copyright (c) 2013 Guilherme Torres. All rights reserved.
//

#ifndef TD_sequence_h
#define TD_sequence_h

#include <cstdint>
typedef struct ulong3
{
  unsigned long int x, y, z;
} sequence_t;

#define SEQ_DIM_BITS_SIZE sizeof(unsigned long int) * 8

void print_sequence(sequence_t seq);
int __attribute__((target(mic))) dist_sequence(sequence_t seq1,sequence_t seq2);

sequence_t __attribute__((target(mic))) copy_sequence(sequence_t seq);
sequence_t __attribute__((target(mic))) sum_sequence(sequence_t seq1,sequence_t seq2);
sequence_t __attribute__((target(mic))) div_sequence(sequence_t seq1, unsigned long div);

#endif
