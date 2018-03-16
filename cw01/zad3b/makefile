CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=blocktable
LIBPATH=./../zad1
PROGNAME=timemeasurement
PROGPATH=./../zad2
FILE=raport3b.txt

all:
	make one_all optim=0
	make one_all optim=1
	make one_all optim=2
	make one_all optim=s

one_all: static shared dynamic

static: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -static $(LDFLAGS) -L$(LIBPATH) -l$(LIBNAME) -o $(PROGNAME)_$@_O$(optim) -O$(optim)

shared: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -Wl,-rpath=$(LIBPATH) $(LDFLAGS) -L$(LIBPATH) -l$(LIBNAME) -o $(PROGNAME)_$@_O$(optim) -O$(optim)

dynamic: $(PROGPATH)/$(PROGNAME).c 

	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -D DLL -ldl -o $(PROGNAME)_$@_O$(optim) -O$(optim)

one_raport: one_all
	echo '\n STATIC LIBRARY \n' $(optim) 'optimization' | tee -a $(FILE)
	./$(PROGNAME)_static_O$(optim) --static -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)
	./$(PROGNAME)_static_O$(optim) --dynamic -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)

	echo '\n SHARED LIBRARY \n' $(optim) 'optimization' | tee -a $(FILE)
	./$(PROGNAME)_shared_O$(optim) --static -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)
	./$(PROGNAME)_shared_O$(optim) --dynamic -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)


raport: 
	make one_raport optim=0
	make one_raport optim=1
	make one_raport optim=2
	make one_raport optim=s

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)*

