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
  fp = fopen(argv[2], "rb");
  read_quant_file(fp, &qm);
  fclose(fp);

  const char* dest = argv[4];

  // take the pgm file, apply the DCT, and then write to disk
  dct_write_file(dest, q, &qm, &pg);

  return 0;
}
