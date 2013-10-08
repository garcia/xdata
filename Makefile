CC=gcc
CFLAGS=-std=c99
OUTPUT=example
C=example.c xdata/xdata.c

all: $(C)
	$(CC) -o $(OUTPUT) $(C) $(CFLAGS)
