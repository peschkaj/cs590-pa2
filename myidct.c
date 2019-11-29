/*  myidct.c - IDCT program to decompress dct formatted images
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
  if (argc < 4) {
    //printf("USAGE: myidct source quantfile qscale output\n");
    printf("USAGE: myidct source quantfile output\n");
    exit(1);
  }

  const char* src = argv[1];
  dct_file df;

  if (dct_read_file(src, &df) == 0) {
    printf("file is %d x %d\n", df.header.xsize, df.header.ysize);
  } else {
    printf("Something went horribly wrong.\n");
  }

  // read quantization matrix from quantfile
  quantization_matrix qm; 
  read_quant_file(argv[2], &qm);
  
  const char* dest = argv[3];
  printf("dest: %s\n", dest); 

  pgm_file pg;
  dct_to_pgm(&qm, &pg, &df);

  pg.fp = fopen(dest, "wb");

  if (!pg.fp) {
    printf("Unable to open '%s'\n", dest);
    exit(-1);
  }

  // take the pgm file, apply the DCT, and then write to disk
  pgm_write_file(&pg);

  return 0;
}
