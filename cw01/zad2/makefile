CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=blocktable
LIBPATH=./../zad1
PROGNAME=timemeasurement
FILE=raport2.txt

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -static -L$(LIBPATH) -l$(LIBNAME) -o $@

raport: $(PROGNAME)
	./$(PROGNAME) --dynamic -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)
	./$(PROGNAME) --static -n 10000000 -l 20 -c -i 10000000 -s 130 -d 1000000 -a 1000000 | tee -a $(FILE)


.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)
