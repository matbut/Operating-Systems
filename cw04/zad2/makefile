CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME_1=child
PROGNAME_2=parent

all: $(PROGNAME_1) $(PROGNAME_2)

$(PROGNAME_1): $(PROGNAME_1).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(PROGNAME_2): $(PROGNAME_2).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

test: all
	./$(PROGNAME_2) -p 10 -w 5 -curse


.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME_1) $(PROGNAME_2)
 

