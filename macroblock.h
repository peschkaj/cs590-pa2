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
