#pragma once

#ifndef DCT_H_
#define DCT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "byte.h"
#include "macro.h"
#include "pgm.h"


void
dct_write_body(FILE* restrict fp, dct_file* restrict pg) {
  // Apply the DCT function to each macro block or something
}

void
dct_write_header(FILE* restrict fp, pgm_file* restrict pg) {
  printf("MYDCT\n");
  printf("%d %d\n", pg->header.xsize, pg->header.ysize);
  printf("Qvalue\n");
}

void
dct_write_file(FILE* fp, pgm_file* restrict pg) {
  dct_write_header(fp, pg);
  dct_write_body(fp, pg);
}


#endif
