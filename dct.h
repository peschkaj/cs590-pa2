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

typedef struct {
  int32_t dcts[BLOCK_SIZE][BLOCK_SIZE];
} dct_block;

typedef struct {
  dct_block blocks[2][2];
} dct_macroblock;

typedef struct {
  uint32_t xsize;
  uint32_t ysize;
  double qvalue;
} dct_header;

typedef struct {
  dct_header header;
  dct_macroblock** macroblocks;
} dct_file;

typedef struct {
  uint32_t x;
  uint32_t y;
} dct_order;

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
      // process each block
      block* src_b = &src_mb->blocks[i][j];
      dct_block* dest_b = &dest_mb->blocks[i][j];

      for (uint32_t u = 0; u < BLOCK_SIZE; u++) {
        for (uint32_t v = 0; v < BLOCK_SIZE; v++) {
          double sum = 0.0;

          for (uint32_t x = 0; x < BLOCK_SIZE; x++) {
            for (uint32_t y = 0; y < BLOCK_SIZE; y++) {
              sum += cos((((2.0 * x) + 1.0) * (x * pi)) / (16.0)) *
                     cos((((2.0 * y) + 1.0) * (y * pi)) / (16.0)) *
                     src_b->bytes[x][y];
            }
          }

          double ci = (i == 0) ? RECIP_ROOT_TWO : 1;
          double cj = (j == 0) ? RECIP_ROOT_TWO : 1;
          double dct =
              clamp(((sum * ci * cj) / (4.0 * q * qm->quant_factor[u][v])),
                    128.0, -128.0);
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
  df->macroblocks = (dct_macroblock**)malloc(rows * sizeof(macroblock*));

  for (uint32_t i = 0; i < rows; i++) {
    df->macroblocks[i] = (dct_macroblock*)malloc(cols * sizeof(macroblock));
  }

  // run DCT on each macroblock
  dct_process_macroblocks(q, qm, pg, df);
}

void
dct_write_body(FILE* fp, dct_file* restrict df) {
  // Apply the DCT function to each macro block or something
}

void
dct_write_header(FILE* fp, dct_file* restrict df) {
  fprintf(fp, "MYDCT\n");
  fprintf(fp, "%d %d\n", df->header.xsize, df->header.ysize);
  fprintf(fp, "%f\n", df->header.qvalue);
}

void
dct_write_file(FILE* fp, double q, quantization_matrix* restrict qm,
               pgm_file* restrict pg, dct_file* restrict df) {
  dct_from_pgm(q, qm, pg, df);
  dct_write_header(fp, df);
  dct_write_body(fp, df);
}

#endif
