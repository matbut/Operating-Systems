CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
PROGNAME=batch_processing
CPU_DESTROYER=cpu_destroyer
MEM_DESTROYER=memory_destroyer
FILE_CPU=sample_cpu.txt
FILE_MEM=sample_mem.txt
PROGPATH=./../zad3a/

all: cpu mem $(PROGNAME)

$(PROGNAME): $(PROGPATH)$(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

cpu: $(CPU_DESTROYER).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $(CPU_DESTROYER)

mem: $(MEM_DESTROYER).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $(MEM_DESTROYER)

test: all
	./$(PROGNAME) $(FILE_MEM) 10 20
	./$(PROGNAME) $(FILE_CPU) 2 200
	

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME) $(CPU_DESTROYER) $(MEM_DESTROYER)

