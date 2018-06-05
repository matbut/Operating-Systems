CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=-lm -lpthread
PROGNAME=filter
INPUT_FILE=brain.ascii.pgm
RAPORT=raport.txt

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)

one_test: $(PROGNAME) 
	./$(PROGNAME) $(THREADS) $(INPUT_FILE) $(FILTER) brain$(NUM).ascii.pgm | tee -a $(RAPORT)

part_test: $(PROGNAME)
	echo '\n$(NUM) Filter $(FILTER) \n' | tee -a $(RAPORT) 
	make one_test THREADS=1 
	make one_test THREADS=2 
	make one_test THREADS=4 
	make one_test THREADS=8 

test: $(PROGNAME) 
	make part_test NUM=1 FILTER=filter1.fltr
	make part_test NUM=2 FILTER=filter2.fltr
	make part_test NUM=3 FILTER=filter3.fltr
