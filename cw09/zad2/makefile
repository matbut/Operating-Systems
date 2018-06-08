CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=-lpthread
PROGNAME=producer_consumer_problem
CONFIG=config

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME) 

test1: $(PROGNAME)
	./$(PROGNAME) $(CONFIG)1

test2: $(PROGNAME)
	./$(PROGNAME) $(CONFIG)2

