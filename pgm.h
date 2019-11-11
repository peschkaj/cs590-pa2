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

#define DEBUG 1

typedef struct {
  uint32_t xsize;
  uint32_t ysize;
} pgm_header;

typedef struct {
  pgm_header header;
  macroblock** macroblocks;
} pgm_file;

static int
pgm_read_header(FILE* restrict fp, pgm_file* restrict f) {
  char* buf;
  size_t len = 0;

  // first line
  if (getline(&buf, &len, fp) == -1) {
    return -1;
  }

  // second line
  if (getline(&buf, &len, fp) == -1) {
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
  if (getline(&buf, &len, fp) == -1) {
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
  fread(buf, sizeof(byte), to_read, fp);

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
#endif
