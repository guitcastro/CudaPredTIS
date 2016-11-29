#include "io.h"
#include <assert.h>

#if BINARY_OUT

extern int mpi_rank;

//assumes little endian
void printBits(FILE *out,unsigned long int * ptr)
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
      fprintf(out,"%u", byte);
    }
  }

}

void printSpecialCase(FILE *out,unsigned long int *ptr){
  printBits(out, ptr);
}
#else
unsigned char baseForBinary(unsigned char b){
  if (b == 1)
  return 'a';
  if (b == 2)
  return 'c';
  if (b == 4)
  return 'g';
  if (b == 8)
  return 't';
  assert(1); // no expected
  return '\0';
}
void printBits(FILE *out,void const * const ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  size_t size = sizeof(unsigned long int);
  int i;
  unsigned char byte;
  for (i=size-1;i>=0;i--)
  {
    byte = b[i] >> 4;
    fprintf(out,"%c", baseForBinary(byte));
    byte = b[i] &= 0xf;
    fprintf(out,"%c", baseForBinary(byte));
  }
  fputs("",out);
}

void printSpecialCase(FILE *out,unsigned long int *ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  size_t size = sizeof(unsigned int); // only half of the bits will be used on Z
  int i;

  // remove caracteristic bits
  unsigned long int threeLast = ((*ptr) & 0xFFFUL);
  *ptr >>= 12;

  unsigned char byte;
  for (i=size-1;i>=0;i--)
  {
    byte = b[i] >> 4;
    fprintf(out,"%c", baseForBinary(byte));
    byte = b[i] &= 0xf;
    fprintf(out,"%c", baseForBinary(byte));
  }
  fputs(" ",out);
  byte = threeLast >> 8;
  if (byte == 1){
    fprintf(out,"%d",1);
  }else if (byte == 2){
    fprintf(out,"%d",0);
  }
  byte = (threeLast >> 4) & 0xF;
  if (byte == 1){
    fprintf(out,"%d",1);
  }else if (byte == 2){
    fprintf(out,"%d",0);
  }
  byte = threeLast & 0xF;
  if (byte == 1){
    fprintf(out,"%d",1);
  }else if (byte == 2){
    fprintf(out,"%d",0);
  }
}
#endif

void printBits2(unsigned long int * ptr)
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

void read_data(char *file) {
  size_t line_size = 255;

  FILE *in = fopen(file,"r");
  char *line = (char*)calloc(255,sizeof(char));

  if(!in) {
    printf("Invalid file!\n");
    exit(0);
  } else {
    printf("Reading file %s\n", file);
  }

  //Count lines
  data_size = 0;
  rewind(in);

  while(!feof(in)) {
    getline(&line,&line_size,in);

    if(line[0] != '\n'){
      data_size++;
    }
  }

  printf("Number of lines = %lu\n", data_size);

  //Read objects
  data = (sequence_t*)calloc(data_size,sizeof(sequence_t));
  rewind(in);

  size_t current_line = 0;
  while(!feof(in)) {
    getline(&line,&line_size,in);
    if(strlen(line) > 0){

      char subbuff[64];

      memcpy(subbuff, &line[0], 64);
      unsigned long int x =  strtoul(subbuff, NULL,2);
      memcpy(subbuff, &line[64], 64);

      unsigned long int y =  strtoul(subbuff, NULL,2);
      memset(subbuff, '\0', 64);
      memcpy(subbuff, &line[128], strlen(line) - 128 + 1);

      unsigned long int z =  strtoul(subbuff, NULL , 2);

      sequence_t seq = sequence_t { x,y,z };
      data[current_line] = seq;
      current_line++;
    } else if (current_line + 1 < data_size) {
      printf("Error parsing line: %zu\n", current_line);
    }
  }

  printf("Finish read input file\n");

  fclose(in);
}

void write_nearest_objects(char *file) {

  #if USE_MPI
  if (mpi_rank != 0) {
    return;
  }
  #endif


  FILE *out = fopen(file,"w");
  int nearest;
  float distance,min_distance;

  if(!out) {
    printf("Invalid file!\n");
    exit(0);
  }

  //For each centroid
  for(size_t i = 0; i < clusters;i++) {
    nearest = -1;
    min_distance = FLT_MAX;

    //Find the nearest object
    for(size_t j = 0;j < data_size;j++) {
      if(label[j] != (int)i) {
        continue;
      }

      distance = dist_sequence (data[j],centroids[i]);

      if(distance < min_distance) {
        nearest = j;
        min_distance = distance;
      }
    }

    if(nearest == -1) {
      nearest = i * data_size/clusters;
    }

    printBits(out,&data[nearest].x);
    fprintf(out," ");
    printBits(out,&data[nearest].y);
    fprintf(out," ");
    printSpecialCase(out,&data[nearest].z);
    fprintf(out," ");
    fprintf(out,"\n");
  }

  fclose(out);
}
