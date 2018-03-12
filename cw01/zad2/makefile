CC=gcc
CFLAGS=-Wall -std=c99 -I.
PROG_NAME=timemeasurement
LIB=../zad1/libblocktable.a

#first default rule
all: build run clean

build: $(PROG_NAME).c
	#Compile (if file change)
	$(CC) -o $(PROG_NAME) $(PROG_NAME).c $(LIB) $(CFLAGS)

run:
	./$(PROG_NAME) 3 3 static 3 3

clean:
	rm -f *.o


