#pragma once

#ifndef QUANT_H_
#define QUANT_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "byte.h"

typedef struct {
  uint32_t quant_factor[8][8];
} quantization_matrix;

void
read_quant_file(FILE* restrict fp, quantization_matrix* restrict qm) {
  char* buf = NULL;
  const char token[] = " ";
  size_t len = 0;
  uint32_t current_line = 0;

  // read each line
  while (getline(&buf, &len, fp) == -1) {
    char* quants = strtok(buf, token);
    // assume 8 entries per line
    for (size_t i = 0; i < 8; i++) {
      qm->quant_factor[current_line][i] = atoi(&quants[i]);
    }
    current_line++;
  }
}

#endif
