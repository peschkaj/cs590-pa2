.DEFAULT_GOAL := mydct

CC = gcc
CCFLAGS = -m64 -march=native --std=gnu11 -Og -g -Wall -Wextra

clean:
	rm -f mydct

mydct:
	$(CC) $(CCFLAGS) -o mydct mydct.c -lm

.PHONY: clean mydct
