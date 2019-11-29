.DEFAULT_GOAL := all

CC = gcc
CCFLAGS = -m64 -march=native --std=gnu11 -Og -g -Wall -Wextra

clean:
	rm -f myDCT myIDCT

all: mydct myidct

mydct:
	$(CC) $(CCFLAGS) -o myDCT mydct.c -lm

myidct:
	$(CC) $(CCFLAGS) -o myIDCT myidct.c -lm

.PHONY: clean mydct myidct
