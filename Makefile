CC = gcc
CCFLAGS = --std=gnu11 -g -Wall -Wextra

clean:
	rm -f cjpg

cjpg:
	$(CC) $(CCFLAGS) -o cjpg cjpg.cjpg

.PHONY: clean cjpg