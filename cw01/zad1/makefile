CC=gcc
CFLAGS=-Wall -std=c11  -ggdb
CPPFLAGS=
LIBNAME=blocktable

all: static shared clean

static:

	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(LIBNAME).c -o $(LIBNAME).o

	ar rcs lib$(LIBNAME).a $(LIBNAME).o 

shared:

	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -shared $(LIBNAME).c -o lib$(LIBNAME).so  

.PHONY: clean 

clean:
	rm -rf *.o


