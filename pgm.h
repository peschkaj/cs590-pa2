#pragma once

#ifndef PGM_H_
#define PGM_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char byte;

typedef struct {
  uint32_t xsize;
  uint32_t ysize;
} pgm_header;

typedef struct {
  pgm_header header;
  byte* bytes;
} pgm_file;

int
pgm_read_header(FILE* fp, pgm_file* f) {
  char* buf;
  size_t len = 0;

  
  // first line
  if(getline(&buf, &len, fp) == -1) {
    return -1;
  }

  // second line
  if(getline(&buf, &len, fp) == -1) {
    return -1;
  }

  // split the string and fill the pgm_header
  char token[] = " ";

  const char* bits = strtok(buf, token);
  f->header.xsize = atoi((const char*)bits);
  bits = strtok(buf, token);
  f->header.ysize = atoi((const char*)bits);

  // third line
  // we read the third line of the header so that the file pointer is now
  // ready for us to read the entire body of the file in one go
  if(getline(&buf, &len, fp) == -1) {
    return -1;
  }

  return 0;
}

int
pgm_read_body(FILE* fp, pgm_file* f) {
  // get current position in the file
  long pos = ftell(fp);

  // get the remaining size of the file
  fseek(fp, 0L, SEEK_END);
  long bufsize = ftell(fp);

  // reset position in file
  fseek(fp, pos, SEEK_SET);

  long to_read = bufsize - pos + 1;

  // malloc a buffer big enough to hold memory
  char* buf = (char*)malloc(to_read);

  // read the entire file into the buffer
  fread(buf, sizeof(byte), to_read, fp);

  if (ferror(fp) != 0) {
    free(buf);
    return -1;
  }

  // TODO create a function to parse this into a macroblock
  f->bytes = (byte*)strdup(buf);
  
  return 0;
}

int
pgm_read_file(FILE* fp, pgm_file* f) {
  if (pgm_read_header(fp, f) < 0) {
    return - 1;
  }

  if (pgm_read_body(fp, f) < 0) {
    return - 1;
  }

  return 0;
}
#endif