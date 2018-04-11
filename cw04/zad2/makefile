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

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME_1) $(PROGNAME_2)
 

