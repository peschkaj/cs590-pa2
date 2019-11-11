/*  macro.h - various macro functions
    Copyright (C) 2019 Jeremiah Peschka

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

#include <stdio.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define debug(fmt, ...)                                                   \
  ({                                                                      \
    do {                                                                  \
      if (DEBUG) printf("%s\t%d\t" fmt, __func__, __LINE__, __VA_ARGS__); \
    } while (0);                                                          \
  })

#define debug_printf(x)    \
  ({                       \
    do {                   \
      if (DEBUG) printf x; \
    } while (0);           \
  })
