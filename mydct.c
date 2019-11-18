#include <stdlib.h>
#include <stdio.h>
#include "pgm.h"
#include "dct.h"
#include "quant.h"

int 
main(int argc, char const *argv[]) 
{
  if (argc < 5) {
    printf("USAGE: mydct source quantfile qscale output\n");
    exit(1);
  }

  printf("BEIGN\n"); 
  const char* src = argv[1];
  FILE* fp = fopen(src, "rb");
  pgm_file pg;

  if (pgm_read_file(fp, &pg) == 0) {
    printf("file is %d x %d\n", pg.header.xsize, pg.header.ysize);
  } else {
    printf("Something went horribly wrong.\n");
  }

  fclose(fp); 

  // read qscale from command line
  double q = atof(argv[3]);

  // read quantization matrix from quantfile
  quantization_matrix qm; 
  printf("open quant file\n"); 
  fp = fopen(argv[2], "rb");
  read_quant_file(fp, &qm);
  fclose(fp);
  printf("close quant file\n"); 
  
  // open destination for writing
  
  const char* dest = argv[4];
  printf("dest is %s\n", dest); 
  fp = fopen(dest, "w");
  if(fp == NULL) { 
	  printf("FILE POINER IS NULL\n"); 
  }
  printf("after fopen\n"); 

  // take the pgm file, apply the DCT, and then write to disk
  dct_file df;
  //Overwriting the file pointer at some point
  dct_write_file(fp, q, &qm, &pg, &df);
  fclose(fp);

  return 0;
}
