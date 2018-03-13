CC=gcc
CFLAGS=-Wall -std=c11 
CPPFLAGS=
LDFLAGS=
LIBNAME=blocktable
LIBPATH=./../zad1
PROGNAME=timemeasurement

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -static -L$(LIBPATH) -l$(LIBNAME) -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)
