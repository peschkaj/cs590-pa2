/*  mydct.c - DCT program to compress pgm formatted images
    Copyright (C) 2019 Jeremiah Peschka and Joel Lilja

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
  read_quant_file(argv[2], &qm);
  
  const char* dest = argv[4];

  // take the pgm file, apply the DCT, and then write to disk
  dct_write_file(dest, q, &qm, &pg);

  return 0;
}
