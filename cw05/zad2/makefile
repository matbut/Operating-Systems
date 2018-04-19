CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
SLAVE=slave
MASTER=master
FIFO=fifo

all: $(SLAVE) $(MASTER)

$(SLAVE): $(SLAVE).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(MASTER): $(MASTER).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(SLAVE) $(MASTER) $(FIFO)

test: $(MASTER) $(SLAVE)
	./$(MASTER) $(FIFO) &
	./$(SLAVE) $(FIFO) 3 &
	./$(SLAVE) $(FIFO) 9 &
	./$(SLAVE) $(FIFO) 5 &
