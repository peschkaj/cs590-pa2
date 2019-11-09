#pragma once

#ifndef PGM_H_
#define PGM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "byte.h"
#include "macroblock.h"

typedef struct {
  uint32_t xsize;
  uint32_t ysize;
} pgm_header;

typedef struct {
  pgm_header header;
  macroblock** macroblocks;
  byte* bytes;
} pgm_file;

int
pgm_read_header(FILE* fp, pgm_file* f) {
  char* buf;
  size_t len = 0;

  
  // first line
  if(getline(&buf, &len, fp) == -1) {
    return -1;
  }

  // second line
  if(getline(&buf, &len, fp) == -1) {
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
  if(getline(&buf, &len, fp) == -1) {
    return -1;
  }

  return 0;
}

static void
pgm_read_block(byte* bytes, pgm_file* f, block* b, uint32_t mb_row, uint32_t mb_col, uint32_t b_row, uint32_t b_col) {
  uint32_t start_y = (mb_row * f->header.xsize) + (b_row * BLOCK_SIZE);
  uint32_t start_x = (mb_col * MACROBLOCK_SIZE) + (b_col * BLOCK_SIZE);
  for (uint32_t i = 0; i < BLOCK_SIZE; i++) {
    for (uint32_t j = 0; j < BLOCK_SIZE; j++) {
      //                     row position in file                 column
      b->bytes[i][j] = bytes[(start_y + (i * f->header.xsize)) + start_x + j];
    }
  }
}

// Read a macroblock from 
//   an array of bytes
//   into a macroblock
//   at the position row, col
// This assumes that x and y are already at some position evenly divisible by 16
static void
pgm_read_macroblock(byte* bytes, pgm_file* f, macroblock* mb, uint32_t row, uint32_t col) {
  for (uint32_t i = 0; i < 2; i++) {
    for (uint32_t j = 0; j < 2; j++) {
      block* b = &(mb->blocks[i][j]);

      pgm_read_block(bytes, f, b, row, col, i, j);
    }
  }
}

static void
pgm_bytes_to_macroblock(byte* bytes, pgm_file* f) {
  uint32_t width = f->header.xsize;
  uint32_t height = f->header.ysize;
  // how many macroblocks do we need horizontally?
  uint32_t rows = height / MACROBLOCK_SIZE;
  // how many macroblocks do we need vertically?
  uint32_t cols = width / MACROBLOCK_SIZE;

  f->macroblocks = (macroblock**)malloc(rows * sizeof(macroblock *));

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

int
pgm_read_body(FILE* fp, pgm_file* f) {
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

  // TODO create a function to parse the body into macroblocks
  f->bytes = (byte*)strdup(buf);

  free(buf);
  
  return 0;
}

int
pgm_read_file(FILE* fp, pgm_file* f) {
  if (pgm_read_header(fp, f) < 0) {
    return - 1;
  }

  if (pgm_read_body(fp, f) < 0) {
    return - 1;
  }

  return 0;
}
#endif