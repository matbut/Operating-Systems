CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME=directory_data_search

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)

test: $(PROGNAME)
	./$(PROGNAME) ../ "<" 2018-05-01

