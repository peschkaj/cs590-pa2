/*  quant.h - quantization matrix
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

#ifndef QUANT_H_
#define QUANT_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "byte.h"
#include "macro.h"

typedef struct {
  uint32_t quant_factor[8][8];
} quantization_matrix;

void
read_quant_file(FILE* restrict fp, quantization_matrix* restrict qm) {
  char* buf = NULL;
  const char token[] = " ";
  size_t len = 0;
  uint32_t current_line = 0;
  ssize_t read = 0;

  // read each line
  while ((read = getline(&buf, &len, fp)) == -1) {
    printf("retrieved line of length: %zu\n", read);
    char* quants = strtok(buf, token);
    // assume 8 entries per line
    for (size_t i = 0; i < 8; i++) {
      qm->quant_factor[current_line][i] = atoi(&quants[i]);
      printf("%d:%d %d\t", current_line, (int)i, qm->quant_factor[current_line][i]);
    }
    printf("\n");
    current_line++;
  }
}

#endif
