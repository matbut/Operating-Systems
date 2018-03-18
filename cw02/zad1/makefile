CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=errorlibstatic
LIBPATH=./../../errorlib
PROGNAME=test

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -L$(LIBPATH) -l$(LIBNAME) -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)
