.DEFAULT_GOAL := mydct

CC = gcc
CCFLAGS = -m64 -march=native --std=gnu11 -g -Wall -Wextra

clean:
	rm -f cjpg

mydct:
	$(CC) $(CCFLAGS) -o mydct mydct.c

.PHONY: clean cjpg
