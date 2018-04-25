CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
SLAVE=client
MASTER=server

all: $(SLAVE) $(MASTER)

$(SLAVE): $(SLAVE).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(MASTER): $(MASTER).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(SLAVE) $(MASTER) 

test: $(MASTER) $(SLAVE)
	./$(MASTER) &
	./$(SLAVE) &
	./$(SLAVE) &
	./$(SLAVE) &
