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

#ip route get 8.8.8.8 | awk '{print $NF; exit}'
#./server 8084 janina
#./client waldemar INET 192.168.0.61 8084 
#./client janusz UNIX janina 

	
