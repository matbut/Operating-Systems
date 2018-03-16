CC=gcc
CFLAGS=-Wall -std=c11  -ggdb
CPPFLAGS=
LIBNAME=blocktable

all: static shared clean

static:

	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(LIBNAME).c -o $(LIBNAME).o

	ar rcs lib$(LIBNAME)static.a $(LIBNAME).o 

shared:

	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -shared $(LIBNAME).c -o lib$(LIBNAME)shared.so  

.PHONY: clean 

clean:
	rm -rf *.o 



