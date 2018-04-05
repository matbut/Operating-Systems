CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME=batch_processing
FILE=sample.txt

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

test: $(PROGNAME)
	./$(PROGNAME) $(FILE) 10 20
	./$(PROGNAME) $(FILE) 2 200
	
.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)

