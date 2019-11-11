#include <stdlib.h>
#include <stdio.h>
#include "pgm.h"
#include "dct.h"

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

  // take the pgm file, apply the DCT, and then write to disk
  const char* dest = argv[4];
  fp = fopen(dest, "wb");
  dct_write_file(fp, &pg);
  fclose(fp);

  return 0;
}
