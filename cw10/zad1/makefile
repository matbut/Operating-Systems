CC=gcc
CFLAGS=-Wall -std=c11 -ggdb 
LIBFLAGS=-lpthread
SLAVE=client
MASTER=server
FILE=commands.txt

all: $(MASTER) $(SLAVE)

$(SLAVE): $(SLAVE).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LIBFLAGS)

$(MASTER): $(MASTER).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LIBFLAGS)

.PHONY: clean

clean:
	-rm -rf *.o $(SLAVE) $(MASTER) 

	
