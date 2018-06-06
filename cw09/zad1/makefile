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

test: $(PROGNAME)
	./$(PROGNAME) $(CONFIG)


