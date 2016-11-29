//
//  sequence.c
//  TD
//
//  Created by Guilherme Torres on 9/7/13.
//  Copyright (c) 2013 Guilherme Torres. All rights reserved.
//

#include <stdio.h>
#include "sequence.h"




#ifndef __CUDACC__
__host__
__device__
#endif
void print_binary(void const * const ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  size_t size = sizeof(unsigned long int);
  int i, j;

  for (i=size-1;i>=0;i--)
  {
    for (j=7;j>=0;j--)
    {
      byte = b[i] & (1<<j);
      byte >>= j;
      printf("%u", byte);
    }
  }
}

#ifndef __CUDACC__
__host__
__device__
#endif
void print_sequence(sequence_t seq)
{
  print_binary(&seq.x);
  print_binary(&seq.y);
  print_binary(&seq.z);
}

#ifndef __CUDACC__
__host__
__device__
#endif
sequence_t copy_sequence(sequence_t seq){
  return make_ulong3(seq.x,seq.y,seq.z);
}

#ifndef __CUDACC__
__host__
__device__
#endif
int  dist_sequence(sequence_t seq1,sequence_t seq2){
  unsigned long int xDiff = seq1.x ^ seq2.x;
  unsigned long int yDiff = seq1.y ^ seq2.y;
  unsigned long int zDiff = seq1.z ^ seq2.z;
  return __builtin_popcountl(xDiff) + __builtin_popcountl(yDiff) + __builtin_popcountl(zDiff);
}

sequence_t  sum_sequence(sequence_t seq1,sequence_t seq2){
  return make_ulong3(seq1.x + seq2.x,seq1.y + seq2.y,seq1.z + seq2.z);
}

sequence_t div_sequence(sequence_t seq1, unsigned long div){
  return make_ulong3(seq1.x / div , seq1.y / div, seq1.z / div);
}
