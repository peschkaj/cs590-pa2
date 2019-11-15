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

typedef struct {
  int32_t dcts[BLOCK_SIZE][BLOCK_SIZE];
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



void
dct_process_macroblock(double q, quantization_matrix* restrict qm,
                       macroblock* src_mb, dct_macroblock* dest_mb) {
  for (uint32_t i = 0; i < MACROBLOCK_ROWS; i++) {
    for (uint32_t j = 0; j < MACROBLOCK_COLS; j++) {
      double sum = 0.0;
      block* src_b = &src_mb->blocks[i][j];

      for (uint32_t x = 0; x < BLOCK_SIZE; x++) {
        for (uint32_t y = 0; y < BLOCK_SIZE; y++) {
          sum += cos((((2.0 * x) + 1.0) * (x * M_PI)) / (16.0)) *
                 cos((((2.0 * y) + 1.0) * (y * M_PI)) / (16.0)) *
                 src_b->bytes[x][y];
        }
      }

      double ci = (i == 0) ? RECIP_ROOT_TWO : 1;
      double cj = (j == 0) ? RECIP_ROOT_TWO : 1;
      double dct = clamp(((sum * ci * cj) / (4.0 * q * qm->quant_factor[i][i])),
                         128.0, -128.0);
      dct += 127;

      // not sure where the result of the DCT should go...
      //dest_mb->blocks[i][j]
    }
  }

  /* some potentially bad DCT code
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
          double sum = 0;
          for (int y = 0; y < N; y++) {
            for (int x = 0; x < N; x++) {
              sum += cos(((2.0 * x + 1) * i * pi) / (2 * N)) *
                     cos(((2.0 * y + 1) * j * pi) / (2 * N)) *
                     pixels[y + yOffset][x + xOffset];
            }
          }
          double cI = (i == 0) ? 0.70710678 : 1;
          double cJ = (j == 0) ? 0.70710678 : 1;
          double dct = (sum * cI * cJ / (4 * (quantMatrix[i][j] * qscale)));
          if (dct > 128) {
            dct = 128;
          } else if (dct < -127) {
            dct = -127;
          }
          dct += 127;
          int rDct = round(dct);
          printf("%12f, %5i", dct, rDct);
        }
        printf("\n");
      }
      */
}

void
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
      double_t sum = 0;

      macroblock* src_mb = &(pf->macroblocks[x][y]);
      macroblock* dest_mb = &(df->macroblocks[x][y]);

      dct_macroblock(q, qm, src_mb, dest_mb);
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
  df->macroblocks = (macroblock**)malloc(rows * sizeof(macroblock*));

  for (uint32_t i = 0; i < rows; i++) {
    df->macroblocks[i] = (macroblock*)malloc(cols * sizeof(macroblock));
  }

  // run DCT on each macroblock
  dct_macroblocks(q, qm, pg, df);
}

void
dct_write_body(FILE* fp, dct_file* restrict df) {
  // Apply the DCT function to each macro block or something
}

void
dct_write_header(FILE* fp, dct_file* restrict df) {
  printf("MYDCT\n");
  printf("%d %d\n", df->header.xsize, df->header.ysize);
  printf("%f\n", df->header.qvalue);
}

void
dct_write_file(FILE* fp, double q, quantization_matrix* restrict qm,
               pgm_file* restrict pg, dct_file* restrict df) {
  dct_from_pgm(q, qm, pg, df);
  dct_write_header(fp, df);
  dct_write_body(fp, df);
}




#endif
