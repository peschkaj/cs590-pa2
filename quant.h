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
#include "file.h"
#include "macro.h"

typedef struct {
  uint32_t quant_factor[8][8];
} quantization_matrix;

void
read_quant_file(const char* restrict src, quantization_matrix* restrict qm) {
  unsigned char* buf;
  
  int32_t bytes_read = read_file(src, &buf);

  if (bytes_read < 0) {
    printf("Unable to read from '%s'\n", src);
    exit(-1);
  }

  // now process buf line by line
  uint32_t current_line = 0;
  uint32_t current_value = 0;
  uint32_t pos = 0;
  
  while(*buf != '\0' && current_line < 8) {
    if (*buf == '\n') {
      ++buf;
      current_value = 0;
      pos = 0;
      ++current_line;
    }

    // remove any spaces
    while (*buf == ' ') {
      buf++;
    }

    // convert digits into numbers
    while (*buf >= '0' && *buf <= '9') {
      current_value = (current_value * 10) + (*buf - '0');
      buf++;
    }

    qm->quant_factor[current_line][pos] = current_value;
    current_value = 0;
    pos++;

  }
}

#endif
