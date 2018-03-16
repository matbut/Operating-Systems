CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=blocktable
LIBPATH=./../zad1
PROGNAME=timemeasurement
PROGPATH=./../zad2
FILE=raport3a.txt

all: static shared dynamic

static: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -static $(LDFLAGS) -L$(LIBPATH) -l$(LIBNAME) -o $(PROGNAME)_$@

shared: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -Wl,-rpath=$(LIBPATH) $(LDFLAGS) -L$(LIBPATH) -l$(LIBNAME) -o $(PROGNAME)_$@

dynamic: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -D DLL -ldl -o $(PROGNAME)_$@ 

raport: all
	echo '\n STATIC LIBRARY \n' | tee -a $(FILE)
	./$(PROGNAME)_static --static -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)
	./$(PROGNAME)_static --dynamic -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)

	echo '\n SHARED LIBRARY \n' | tee -a $(FILE)
	./$(PROGNAME)_shared --static -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)
	./$(PROGNAME)_shared --dynamic -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)*
