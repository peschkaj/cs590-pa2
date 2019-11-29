/*  pgm.c - PGM file and functionality
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
#pragma once

#ifndef PGM_H_
#define PGM_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "byte.h"
#include "macro.h"
#include "macroblock.h"
#include "order.h"

#define DEBUG 0

typedef struct {
  uint32_t xsize;
  uint32_t ysize;
} pgm_header;

typedef struct {
  pgm_header header;
  macroblock** macroblocks;
  FILE * fp; 
} pgm_file;

// Read relevant header from a file pointer
static int
pgm_read_header(FILE* restrict fp, pgm_file* restrict f) {
  char* buf = NULL;
  size_t len = 0;
  ssize_t read = 0;

  // first line
  if ((read = getline(&buf, &len, fp)) == -1) {
    return -1;
  }

  // second line
  if ((read = getline(&buf, &len, fp)) == -1) {
    return -1;
  }

  // split the string and fill the pgm_header
  char token[] = " ";

  const char* bits = strtok(buf, token);
  f->header.xsize = atoi((const char*)bits);
  bits = strtok(buf, token);
  f->header.ysize = atoi((const char*)bits);

  // third line
  // we read the third line of the header so that the file pointer is now
  // ready for us to read the entire body of the file in one go
  if ((read = getline(&buf, &len, fp)) == -1) {
    return -1;
  }

  return 0;
}

static void
pgm_read_block(byte* restrict bytes, pgm_file* restrict f, block* restrict b,
               uint32_t mb_row, uint32_t mb_col, uint32_t b_row,
               uint32_t b_col) {
  uint32_t start_y = (mb_row * MACROBLOCK_SIZE) 
                   + ((b_row * BLOCK_SIZE) * f->header.xsize);
  uint32_t start_x = (mb_col * MACROBLOCK_SIZE) + (b_col * BLOCK_SIZE);

  debug_printf(("\tmb[%d][%d]: block[%d][%d]\n", mb_row, mb_col, b_row, b_col));
  debug_printf(("\tstart_y: %4d\tstart_x: %4d\n", start_y, start_x));

  for (uint32_t i = 0; i < BLOCK_SIZE; i++) {
    // Get the current y position by summing:
    //   starting y position
    //   the current row (i) + the width of each row
    uint32_t row = start_y + (i * f->header.xsize);
    debug_printf(("\t\t%4d\t", row));

    for (uint32_t j = 0; j < BLOCK_SIZE; j++) {
      uint32_t col = start_x + j;
      b->bytes[i][j] = bytes[row + col];
      debug_printf(("%3d: %2u ", row + col, b->bytes[i][j]));
    }
    debug_printf(("\n"));
  }
}



// Read a macroblock from
//   an array of bytes
//   into a macroblock
//   at the position row, col
// This assumes that x and y are already at some position evenly divisible by 16
static void
pgm_read_macroblock(byte* restrict bytes, pgm_file* restrict f, macroblock* restrict mb, 
                    uint32_t mb_row, uint32_t mb_col) {
  for (uint32_t i = 0; i < MACROBLOCK_ROWS; i++) {
    for (uint32_t j = 0; j < MACROBLOCK_COLS; j++) {
      block* b = &(mb->blocks[i][j]);
      pgm_read_block(bytes, f, b, mb_row, mb_col, i, j);
    }
  }
}

static void
pgm_bytes_to_macroblock(byte* restrict bytes, pgm_file* restrict f) {
  uint32_t width = f->header.xsize;
  uint32_t height = f->header.ysize;
  // how many macroblocks do we need horizontally?
  uint32_t rows = height / MACROBLOCK_SIZE;
  // how many macroblocks do we need vertically?
  uint32_t cols = width / MACROBLOCK_SIZE;

  f->macroblocks = (macroblock**)malloc(rows * sizeof(macroblock*));

  for (uint32_t i = 0; i < rows; i++) {
    f->macroblocks[i] = (macroblock*)malloc(cols * sizeof(macroblock));
  }

  // now that we've initialized the macroblocks, we can copy data into them
  for (uint32_t i = 0; i < rows; i++) {
    for (uint32_t j = 0; j < cols; j++) {
      macroblock* mb = &(f->macroblocks[i][j]);

      // now read this 16 x 16 chunk into the macroblock
      pgm_read_macroblock(bytes, f, mb, i, j);
    }
  }
}

static int
pgm_read_body(FILE* restrict fp, pgm_file* restrict f) {
  // get current position in the file
  long pos = ftell(fp);

  // get the remaining size of the file
  fseek(fp, 0L, SEEK_END);
  long bufsize = ftell(fp);

  // reset position in file
  fseek(fp, pos, SEEK_SET);

  long to_read = bufsize - pos + 1;

  // malloc a buffer big enough to hold memory
  char* buf = (char*)malloc(to_read);

  // read the entire file into the buffer
  ssize_t read = fread(buf, sizeof(byte), to_read, fp);

  if (read == -1) {
    printf("Unable to read from file in %s\n", __FUNCTION__);
    exit(-1);
  }

  if (ferror(fp) != 0) {
    free(buf);
    return -1;
  }

  // Parse the buffer into macroblocks
  pgm_bytes_to_macroblock((byte*)buf, f);

  free(buf);

  return 0;
}

int
pgm_read_file(FILE* restrict fp, pgm_file* restrict f) {
  if (pgm_read_header(fp, f) < 0) {
    return -1;
  }

  if (pgm_read_body(fp, f) < 0) {
    return -1;
  }

  return 0;
}

// void
// pgm_write_block(pgm_file* pg, block* block) {
//   for (uint32_t i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); ++i) {
//     fprintf(pg->fp, " ");

//     // Use the order array to fine the next position for writing
//     uint32_t y = order[i][0];
//     uint32_t x = order[i][1];
//     fprintf(pg->fp, "%3d", block->bytes[y][x]);

//     // print a new line every 8 DCTs
//     if (i % 8 == 7) {
//       fprintf(pg->fp, "\n");
//     }
//   }
// }

// void
// pgm_write_macroblock(pgm_file* pg, macroblock* macroblock, uint32_t mb_x, uint32_t mb_y){ 
//   for (uint32_t i = 0; i < 2; i++) {
//     for (uint32_t j = 0; j < 2; j++) {
//       fprintf(pg->fp, "%d %d\n", (mb_x * MACROBLOCK_SIZE + j * BLOCK_SIZE),
//               (mb_y * MACROBLOCK_SIZE + i * BLOCK_SIZE));
//       pgm_write_block(pg, &macroblock->blocks[i][j]);
//     }
//   }
// }

void
pgm_write_body(pgm_file * pg) { 
  uint32_t xsize = pg->header.xsize;
  uint32_t ysize = pg->header.ysize;
  macroblock* mb;
  block *b;
  
  for (uint32_t y = 0; y < ysize; y++) {
    // for each "line" of the output image

    for (uint32_t x = 0; x < xsize; x++) {
      // pick out the correct macroblock (which is just the current location)
      uint32_t mb_row = y / MACROBLOCK_SIZE;
      uint32_t mb_col = x / MACROBLOCK_SIZE;
      mb = &pg->macroblocks[mb_row][mb_col];

      // and find the right block in the macroblock
      uint32_t b_row = (y % MACROBLOCK_SIZE) / BLOCK_SIZE;
      uint32_t b_col = (x % MACROBLOCK_SIZE) / BLOCK_SIZE;
      b = &mb->blocks[b_row][b_col];

      uint32_t y_byte = (y % MACROBLOCK_SIZE) % BLOCK_SIZE;
      uint32_t x_byte = (x % MACROBLOCK_SIZE) % BLOCK_SIZE;
      fprintf(pg->fp, "%x", b->bytes[y_byte][x_byte] );
    }
  }
}

void
pgm_write_header(pgm_file * pg) { 
  fprintf(pg->fp, "%s", "P5\n"); 
  fprintf(pg->fp, "%d %d\n", pg->header.xsize, pg->header.ysize); 
  fprintf(pg->fp,  "%s", "255\n"); 
}

void
pgm_write_file(pgm_file* pg) { 
  pgm_write_header(pg); 
  pgm_write_body(pg); 
}
#endif
