CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDLIBS =-lrt -lpthread
SLAVE=client
MAIN=main
MASTER=barber
LIB=hairdresser

all: $(SLAVE) $(MASTER) $(MAIN) 

$(LIB): $(LIB).c $(LIB).h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $(LIB).c -o $(LIB).o $(LDLIBS)
	ar rcs lib$(LIB).a $(LIB).o 

$(SLAVE): $(SLAVE).c $(LIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SLAVE).c -L. -l$(LIB) -o $@ $(LDLIBS)

$(MASTER): $(MASTER).c $(LIB)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MASTER).c -L. -l$(LIB) -o $@ $(LDLIBS)

$(MAIN): $(MAIN).c $(SLAVE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(MAIN).c -o $@ 

.PHONY: clean

clean:
	-rm -rf *.o $(SLAVE) $(MASTER) $(MAIN) lib$(LIB).a

test: all
	./$(MASTER) 3 &
	./$(MAIN) 10 10 &
	
	
