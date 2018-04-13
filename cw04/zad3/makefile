CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME=signal_stat

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

test: $(PROGNAME)
	./$(PROGNAME) 100 1
	./$(PROGNAME) 100 2
	./$(PROGNAME) 100 3

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME) 
 

