#pragma once

#ifndef MACROBLOCK_H_
#define MACROBLOCK_H_

#include "byte.h"

typedef struct {
  byte bytes[8][8];
} block;

typedef struct {
  block blocks[2][2];
} macroblock;

#endif