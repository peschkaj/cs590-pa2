/*  files.h - basic file IO code
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

#include <stdio.h>
#include <stdlib.h>

// read a file into a buffer
int
read_file(char* restrict source, unsigned char** restrict buf) {
  printf("Reading from %s\n", source);

  FILE* fp = fopen(source, "rb");

  if (fp == NULL) {
    printf("Unable to read from source '%s'\n", source);
    exit(1);
  }

  size_t l = 0;

  if (fseek(fp, 0L, SEEK_END) == 0) {
    // get the size of the file
    long bufsize = ftell(fp);
    if (bufsize == -1) {
      printf("Couldn't get the size of '%s'\n", source);
      exit(1);
    }

    // allocate the buffer to the size of the file
    *buf = (unsigned char*)malloc(sizeof(char) * (bufsize + 1));
    if (*buf == NULL) {
      printf("Unable to allocate input buffer. Buy more RAM.\n");
      exit(1);
    }

    // return to the beginning of the file
    if (fseek(fp, 0L, SEEK_SET) != 0) {
      printf("Couldn't seek to beginning of '%s'\n", source);
      exit(1);
    }

    // read the entire file into the buffer
    l = fread(*buf, sizeof(unsigned char), bufsize, fp);

    if (ferror(fp) != 0) {
      printf("Error reading file '%s'\n", source);
      exit(1);
    } else {
      if (l < (size_t)bufsize) {
        *buf[l++] = '\0';
      }
    }
  }

  fclose(fp);

  return l;
}

// write the buffer to a file
int
write_file(char* restrict dest, unsigned char* restrict buf, size_t buf_sz) {
  printf("Writing to %s... ", dest);
  FILE* fp = fopen(dest, "wb");

  if (fp == NULL) {
    printf("Unable to write to %s\n", dest);
    exit(1);
  }

  size_t written = fwrite(buf, 1, buf_sz, fp);
  fclose(fp);

  printf("Wrote %zu bytes to %s\n", written, dest);

  return written;
}
