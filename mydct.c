#include <stdlib.h>
#include <stdio.h>
#include "pgm.h"

int 
main(int argc, char const *argv[]) 
{
  if (argc < 2) {
    printf("USAGE: cpjg source\n");
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

  return 0;
}
