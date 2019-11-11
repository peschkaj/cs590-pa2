CC = gcc
CCFLAGS = -m64 -march=native --std=gnu11 -g -Wall -Wextra

clean:
	rm -f cjpg

cjpg:
	$(CC) $(CCFLAGS) -o cjpg cjpg.c

.PHONY: clean cjpg
