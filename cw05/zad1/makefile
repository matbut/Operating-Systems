CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME=batch_processing
FILE=sample.txt

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)

test: $(PROGNAME) $(FILE)
	./$(PROGNAME) $(FILE)
