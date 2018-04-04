CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=errorlibstatic
LIBPATH=./../../errorlib
PROGNAME=directory_data_search

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -L$(LIBPATH) -l$(LIBNAME) -o $@

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME)

test: $(PROGNAME)
	./$(PROGNAME) ../ "<" 2018-03-21 --sys
	./$(PROGNAME) ../ ">" 2018-03-21 --sys
	./$(PROGNAME) ../ "=" 2018-03-21 --sys
	./$(PROGNAME) ../ "<" 2018-03-21 --lib
	./$(PROGNAME) ../ ">" 2018-03-21 --lib
	./$(PROGNAME) ../ "=" 2018-03-21 --lib
