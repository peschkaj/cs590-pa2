#pragma once

#ifndef FILE_H_
#define FILE_HI

#include <stdio.h>
#include <stdlib.h>

int
read_file(const char* restrict source, unsigned char** restrict buf) {
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

#endif
