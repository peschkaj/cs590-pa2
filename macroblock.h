/*  macroblock.h - the PGM macroblock
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

#ifndef MACROBLOCK_H_
#define MACROBLOCK_H_

#include "byte.h"

#define MACROBLOCK_SIZE 16
#define MACROBLOCK_ROWS 2
#define MACROBLOCK_COLS 2
#define BLOCK_SIZE 8

typedef struct {
  byte bytes[BLOCK_SIZE][BLOCK_SIZE];
} block;

typedef struct {
  block blocks[2][2];
} macroblock;

#endif
