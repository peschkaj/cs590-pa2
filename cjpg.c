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

  pgm_read_header(fp, &pg);

  printf("file is %d x %d\n", pg.header.xsize, pg.header.ysize);

  return 0;
}
