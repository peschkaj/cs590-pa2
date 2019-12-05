//Copyright (C) 2019 Jeremiah Peschka and Joel Lilja
#pragma once

#ifndef SWAP_H_
#define SWAP_H_

#include <stdint.h>

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

#endif 
