CC=gcc
CFLAGS=-Wall -std=c11 -ggdb
CPPFLAGS=
LDFLAGS=
LIBNAME=errorlibstatic
LIBPATH=./../../errorlib
PROGNAME=filesfunctions
FILENAME=ala.txt
COPYNAME=ala_copy.txt
RAPORTNAME=raport.txt

RECORD_LEN_1 = 4 
RECORD_LEN_2 = 512
RECORD_LEN_3 = 4096
RECORD_LEN_4 = 8192

RECORDS_NUM_1 = 6400
RECORDS_NUM_2 = 6400
RECORDS_NUM_3 = 6400
RECORDS_NUM_4 = 6400

$(PROGNAME): $(PROGNAME).c 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -L$(LIBPATH) -l$(LIBNAME) -o $@


raport: $(PROGNAME)
	echo '\n $(RECORDS_NUM_1) records with $(RECORD_LEN_1) length \n' | tee -a $(RAPORTNAME)
	./$(PROGNAME) 	-n $(RECORDS_NUM_1) -l $(RECORD_LEN_1) -f $(FILENAME) -g 
	./$(PROGNAME) --lib -n $(RECORDS_NUM_1) -l $(RECORD_LEN_1) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --lib -n $(RECORDS_NUM_1) -l $(RECORD_LEN_1) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	-rm -rf *.o $(COPYNAME) 
	./$(PROGNAME) --sys -n $(RECORDS_NUM_1) -l $(RECORD_LEN_1) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --sys -n $(RECORDS_NUM_1) -l $(RECORD_LEN_1) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	echo '\n $(RECORDS_NUM_2) records with $(RECORD_LEN_2) length \n' | tee -a $(RAPORTNAME)
	./$(PROGNAME) 	-n $(RECORDS_NUM_2) -l $(RECORD_LEN_2) -f $(FILENAME) -g 
	./$(PROGNAME) --lib -n $(RECORDS_NUM_2) -l $(RECORD_LEN_2) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --lib -n $(RECORDS_NUM_2) -l $(RECORD_LEN_2) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	-rm -rf *.o $(COPYNAME) 
	./$(PROGNAME) --sys -n $(RECORDS_NUM_2) -l $(RECORD_LEN_2) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --sys -n $(RECORDS_NUM_2) -l $(RECORD_LEN_2) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	echo '\n $(RECORDS_NUM_3) records with $(RECORD_LEN_3) length \n' | tee -a $(RAPORTNAME)
	./$(PROGNAME) 	-n $(RECORDS_NUM_3) -l $(RECORD_LEN_3) -f $(FILENAME) -g 
	./$(PROGNAME) --lib -n $(RECORDS_NUM_3) -l $(RECORD_LEN_3) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --lib -n $(RECORDS_NUM_3) -l $(RECORD_LEN_3) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	-rm -rf *.o $(COPYNAME) 
	./$(PROGNAME) --sys -n $(RECORDS_NUM_3) -l $(RECORD_LEN_3) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --sys -n $(RECORDS_NUM_3) -l $(RECORD_LEN_3) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	echo '\n $(RECORDS_NUM_4) records with $(RECORD_LEN_4) length \n' | tee -a $(RAPORTNAME)
	./$(PROGNAME) 	-n $(RECORDS_NUM_4) -l $(RECORD_LEN_4) -f $(FILENAME) -g 
	./$(PROGNAME) --lib -n $(RECORDS_NUM_4) -l $(RECORD_LEN_4) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --lib -n $(RECORDS_NUM_4) -l $(RECORD_LEN_4) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)
	-rm -rf *.o $(COPYNAME) 
	./$(PROGNAME) --sys -n $(RECORDS_NUM_4) -l $(RECORD_LEN_4) -f $(FILENAME) -c $(COPYNAME) | tee -a $(RAPORTNAME)
	./$(PROGNAME) --sys -n $(RECORDS_NUM_4) -l $(RECORD_LEN_4) -f $(COPYNAME) -s | tee -a $(RAPORTNAME)

.PHONY: clean

clean:
	-rm -rf *.o $(PROGNAME) $(COPYNAME) $(FILENAME)
