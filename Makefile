CC=gcc
CFLAGS=
OUTPUT=example
C=example.c xdata/xdata.c

all: $(C)
	$(CC) -o $(OUTPUT) $(C) $(CFLAGS)
