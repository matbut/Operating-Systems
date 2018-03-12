CC=gcc
CFLAGS=-Wall -std=c99 -I.
LIB_NAME=blocktable

#first default rule
all: static shared

static: $(LIB_NAME).c
	#Compile static library (if file change)
	$(CC) -c $(LIB_NAME).c -o $(LIB_NAME).o $(CFLAGS)

	#Build static library .a
	ar rcs lib$(LIB_NAME).a $(LIB_NAME).o 

	make clean

shared: $(LIB_NAME).c
	#Compile shared library .so (if file change)
	$(CC) -fPIC -c $(LIB_NAME).c -o $(LIB_NAME).o $(CFLAGS)
	$(CC) -fPIC -shared -o lib$(LIB_NAME).so $(LIB_NAME).o $(CFLAGS)

	make clean

clean:
	rm -f *.o


