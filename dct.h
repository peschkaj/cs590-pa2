/*  dct.h - JPEG "DCT" functionality
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

#ifndef DCT_H_
#define DCT_H_

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "byte.h"
#include "macro.h"
#include "macroblock.h"
#include "pgm.h"
#include "quant.h"

#define ROOT_TWO sqrt(2.0)
#define RECIP_ROOT_TWO (1 / ROOT_TWO)
#define clamp(x, upper, lower) ({ x > 0 ? min(upper, x) : max(lower, x); })
#define pi M_PI

// Set up a swap function specific for uint32_t
static inline void
swap_uint32_t(uint32_t* x, uint32_t* y) {
  uint32_t t = *x;
  *x = *y;
  *y = t;
}
// A generic byte-by-byte swap
#define swap_generic(x_,y_) do \
   { unsigned char swap_temp[sizeof(x_) == sizeof(y_) ? (signed)sizeof(x_) : -1]; \
     memcpy(swap_temp,&y_,sizeof(x_)); \
     memcpy(&y_,&x_,      sizeof(x_)); \
     memcpy(&x_,swap_temp,sizeof(x_)); \
    } while(0)
// Use the C11 _Generic macro magic to create a general purpose
//  swap function
#define swap(x_, y_) _Generic((x_), \
  uint32_t: swap_uint32_t(&x_, &y_), \
  default:  swap_generic(&x_, &y_)   \
)(x)

typedef struct {
  int32_t dcts[BLOCK_SIZE][BLOCK_SIZE];
} dct_block;

typedef struct {
  dct_block blocks[2][2];
} dct_macroblock;

typedef struct {
  uint32_t xsize;  // width in pixels
  uint32_t ysize;  // height in pixels
  double qvalue;   // user supplied q value
} dct_header;

typedef struct {
  dct_header header;
  FILE* fp;
  dct_macroblock** macroblocks;
} dct_file;

int order[64][2] = {
    {0, 0}, {0, 1}, {1, 0}, {2, 0}, {1, 1}, {0, 2}, {0, 3}, {2, 1},  //  7
    {1, 2}, {3, 0}, {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 5},  // 15
    {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0}, {6, 0}, {5, 1}, {4, 2},  // 23
    {3, 3}, {2, 4}, {1, 5}, {0, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4},  // 31
    {4, 3}, {5, 2}, {6, 1}, {7, 0}, {1, 7}, {2, 6}, {3, 5}, {4, 4},  // 39
    {5, 3}, {6, 2}, {7, 1}, {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3},  // 47
    {7, 2}, {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7}, {4, 7}, {5, 6},  // 55
    {6, 5}, {7, 4}, {7, 5}, {6, 6}, {5, 7}, {6, 7}, {7, 6}, {7, 7}   // 63
};

static void
dct_process_macroblock(double q, quantization_matrix* restrict qm,
                       macroblock* src_mb, dct_macroblock* dest_mb) {
  for (uint32_t i = 0; i < MACROBLOCK_ROWS; i++) {
    for (uint32_t j = 0; j < MACROBLOCK_COLS; j++) {
      block* src_b = &src_mb->blocks[i][j];
      dct_block* dest_b = &dest_mb->blocks[i][j];

      // process each block
      for (uint32_t u = 0; u < BLOCK_SIZE; u++) {
        for (uint32_t v = 0; v < BLOCK_SIZE; v++) {
          double sum = 0.0;
          // printf("----------------\n");
          for (uint32_t x = 0; x < BLOCK_SIZE; x++) {
            for (uint32_t y = 0; y < BLOCK_SIZE; y++) {
              sum += cos((((2.0 * x) + 1.0) * (u * pi)) / (16.0)) *
                     cos((((2.0 * y) + 1.0) * (v * pi)) / (16.0)) *
                     src_b->bytes[x][y];
            }
          }

          double cu = (u == 0) ? RECIP_ROOT_TWO : 1;
          double cv = (v == 0) ? RECIP_ROOT_TWO : 1;
          double dct =
              clamp(((sum * cu * cv) / (4.0 * q * qm->quant_factor[u][v])),
                    128.0, -128.0);
          // debug_printf("q: %f qm: %d sum: %f dct: %f\n", q,
          // qm->quant_factor[u][v], sum, dct);
          dct += 127.0;

          dest_b->dcts[u][v] = round(dct);
        }
      }
    }
  }
}


static void
dct_process_macroblocks(double q, quantization_matrix* restrict qm,
                        pgm_file* restrict pf, dct_file* restrict df) {
  uint32_t width = df->header.xsize;
  uint32_t height = df->header.ysize;

  // how many macroblocks do we need horizontally?
  uint32_t rows = height / MACROBLOCK_SIZE;
  // how many macroblocks do we need vertically?
  uint32_t cols = width / MACROBLOCK_SIZE;


  for (uint32_t x = 0; x < rows; x++) {
    for (uint32_t y = 0; y < cols; y++) {
      macroblock* src_mb = &(pf->macroblocks[x][y]);
      dct_macroblock* dest_mb = &(df->macroblocks[x][y]);

      dct_process_macroblock(q, qm, src_mb, dest_mb);
    }
  }
}

void
dct_from_pgm(double q, quantization_matrix* qm, pgm_file* restrict pg,
             dct_file* restrict df) {
  df->header.qvalue = q;
  df->header.xsize = pg->header.xsize;
  df->header.ysize = pg->header.ysize;

  uint32_t width = df->header.xsize;
  uint32_t height = df->header.ysize;

  // how many macroblocks do we need horizontally?
  uint32_t rows = height / MACROBLOCK_SIZE;
  // how many macroblocks do we need vertically?
  uint32_t cols = width / MACROBLOCK_SIZE;

  // allocate memory for macroblocks
  df->macroblocks = (dct_macroblock**)malloc(rows * sizeof(dct_macroblock*));

  for (uint32_t i = 0; i < rows; i++) {
    df->macroblocks[i] = (dct_macroblock*)malloc(cols * sizeof(dct_macroblock));
  }

  // run DCT on each macroblock
  dct_process_macroblocks(q, qm, pg, df);
}

static void
dct_write_block(dct_file* df, dct_block* block) {
  // do some shit to write a block
  for (uint32_t i = 0; i < (BLOCK_SIZE * BLOCK_SIZE); ++i) {
    fprintf(df->fp, "  ");

    // Use the order array to fine the next position for writing
    uint32_t y = order[i][0];
    uint32_t x = order[i][1];
    fprintf(df->fp, "%d", block->dcts[y][x]);

    // print a new line every 8 DCTs
    if (i % 8 == 7) {
      fprintf(df->fp, "\n");
    }
  }
}

static void
dct_write_macroblock(dct_file* df, dct_macroblock* macroblock, uint32_t mb_x,
                     uint32_t mb_y) {
  for (uint32_t i = 0; i < 2; i++) {
    for (uint32_t j = 0; j < 2; j++) {
      fprintf(df->fp, "%d %d\n", (mb_x * MACROBLOCK_SIZE + j * BLOCK_SIZE),
              (mb_y * MACROBLOCK_SIZE + i * BLOCK_SIZE));
      dct_write_block(df, &macroblock->blocks[i][j]);
    }
  }
}

static void
dct_write_body(dct_file* restrict df) {
  // Apply the DCT function to each macro block or something
  uint32_t xsize = df->header.xsize;
  uint32_t ysize = df->header.ysize;
  uint32_t rows = ysize / MACROBLOCK_SIZE;
  uint32_t cols = xsize / MACROBLOCK_SIZE;
  for (uint32_t i = 0; i < rows; i++) {
    for (uint32_t j = 0; j < cols; j++) {
      dct_write_macroblock(df, &(df->macroblocks[i][j]), i, j);
    }
  }
}

static void
dct_write_header(dct_file* df) {
  fprintf(df->fp, "MYDCT\n");
  fprintf(df->fp, "%d %d\n", df->header.xsize, df->header.ysize);
  fprintf(df->fp, "%f\n", df->header.qvalue);
}

void
dct_write_file(const char* dest, double q, quantization_matrix* restrict qm,
               pgm_file* restrict pg) {
  dct_file df;
  dct_from_pgm(q, qm, pg, &df);

  // open destination for writing
  df.fp = fopen(dest, "w");

  if (!df.fp) {
    printf("Unable to open '%s'\n", dest);
    exit(-1);
  }

  dct_write_header(&df);
  dct_write_body(&df);

  fclose(df.fp);
}

// Read the DCT header information into the dct_file header
// returns 0 on success, -1 otherwise
static int
dct_read_header(dct_file* df) {
  char* buf = NULL;
  size_t len = 0;
  ssize_t read = 0;

  // first line, should be MYDCT
  if ((read = getline(&buf, &len, df->fp)) == -1) {
    return -1;
  }

  if (strcmp(buf, "MYDCT") != 0) {
    return -1;
  }

  // second line is width/height
  if ((read = getline(&buf, &len, df->fp)) == -1) {
    return -1;
  }

  char token[] = "   ";

  const char* bits = strtok(buf, token);
  df->header.xsize = atoi((const char*)bits);
  bits = strtok(buf, token);
  df->header.ysize = atoi((const char*)bits);

  // third line contains the quant factor
  if ((read = getline(&buf, &len, df->fp)) == -1) {
    return -1;
  }

  // we have the quant factor, now parse it from a string to a double
  df->header.qvalue = atof((const char*)buf);

  return 0;
}

// Reads in a block from a DCT file. 
// The expected format is:
// 0 0
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
//   127  127  127  127  127  127  127  127
static int
dct_read_block(FILE* restrict fp, dct_block* restrict b) {
  // Read the next line from fp assuming it's of the form "x y"
  uint32_t x, y;
  if(fscanf(fp, "%d %d", &x, &y) != 2) {
    return -1;
  }

  // read the next eight lines from the body
  for (uint32_t i = 0; i < 8; i++) {
    if (fscanf(fp, "%d %d %d %d %d %d %d %d", &b->dcts[i][0], &b->dcts[i][1],
           &b->dcts[i][2], &b->dcts[i][3], &b->dcts[i][4], &b->dcts[i][5],
           &b->dcts[i][6], &b->dcts[i][7]) != 8) {
      return -1;
    }
  }

  return 0;
}

// Read a macroblock
// Reads a specific macroblock by reading in each of its four blocks
static int
dct_read_macroblock(FILE* restrict fp, dct_macroblock* restrict mb) {
  for (uint32_t y = 0; y < MACROBLOCK_ROWS; y++) {
    for (uint32_t x = 0; x < MACROBLOCK_COLS; x++) {
      if (dct_read_block(fp, &mb->blocks[y][x]) != 0) {
        return -1;
      }
    }
  }

  return 0;
}

// Read the body of fp into df.
// Assumes that dct_read_header has been called first.
static int
dct_read_body(dct_file* restrict df) {
  uint32_t cols = df->header.xsize / MACROBLOCK_SIZE;
  uint32_t rows = df->header.ysize / MACROBLOCK_SIZE;

  df->macroblocks = (dct_macroblock**)malloc(rows * sizeof(dct_macroblock*));

  for (uint32_t y = 0; y < rows; y++) {
    for (uint32_t x = 0; x < cols; x++) {
      df->macroblocks[y] = (dct_macroblock*)malloc(cols * sizeof(dct_macroblock));
      if (dct_read_macroblock(df->fp, &df->macroblocks[y][x]) == -1) {
        return -1;
      }
    }
  }

  return 0;
}

// Read from src into df.
void
dct_read_file(const char* src, dct_file* df) {
  df->fp = fopen(src, "rb");

  if (!df->fp) {
    printf("Unable to open '%s'\n", src);
    exit(-1);
  }

  if (dct_read_header(df) == -1) {
    printf("Unable to read DCT header from '%s'\n", src);
    exit(-1);
  }

  if (dct_read_body(df) == -1) {
    printf("Unable to read DCT body from '%s'\n", src);
    exit(-1);
  }

  fclose(df->fp);
}

void
idct_process_block(double q, quantization_matrix* restrict qm, dct_block* src_b,
                   block* dest_b) {
  // Iterating through the output block so we can inverse the compression
  for (uint32_t x = 0; x < BLOCK_SIZE; x++) {
    for (uint32_t y = 0; y < BLOCK_SIZE; y++) {
      double sum = 0.0;
      // placing result value into src_b->block[x][y]

      for (uint32_t u = 0; u < BLOCK_SIZE; u++) {
        for (uint32_t v = 0; v < BLOCK_SIZE; v++) {
          int32_t ival = src_b->dcts[u][v];
          ival += 127;  // Reset offset

          // Need to multiply by quant value
          ival = ival * 4.0 * qm->quant_factor[u][v] * q;
          // Do idct here now that we have reconstructed value?
          // ival == F(u,v)


          // Getting Cu Cv values
          double cu = (u == 0) ? RECIP_ROOT_TWO : 1;
          double cv = (v == 0) ? RECIP_ROOT_TWO : 1;

          double tval = ival * cu * cv;
          sum += cos((((2.0 * x) + 1.0) * (u * pi)) / (16.0)) *
                 cos((((2.0 * y) + 1.0) * (v * pi)) / (16.0)) * tval;
        }
      }

      sum = sum / 4;
      double oval = clamp(sum, 255, 0);
      dest_b->bytes[x][y] = round(oval);
    }
  }
}

void
idct_process_macroblock(double q, quantization_matrix* restrict qm, dct_macroblock* src_mb, macroblock* dest_mb) {
  for (uint32_t i = 0; i < MACROBLOCK_ROWS; i++) {
    for (uint32_t j = 0; j < MACROBLOCK_COLS; j++) {
	    dct_block* in_block = &(src_mb->blocks[i][j]);
	    block* out_block = &(dest_mb->blocks[i][j]);
	    idct_process_block(q, qm, in_block, out_block);
    }
  }

}

static void
idct_process_macroblocks(double q, quantization_matrix* restrict qm, dct_file *df, pgm_file * pf) {
}
#endif
