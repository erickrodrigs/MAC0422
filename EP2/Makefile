CC		     = gcc
CFLAGS     = -Wall -g
PTHREAD    = -lpthread
SRC		     = src
RM         = rm
#-----------------------

ep2: ep2.o sort.o stack.o
	$(CC) -o ep2 ep2.o sort.o stack.o $(PTHREAD)

ep2.o: $(SRC)/ep2.c $(SRC)/sort.h $(SRC)/cyclist.h $(SRC)/stack.h
	$(CC) $(CFLAGS) -c $(SRC)/ep2.c

sort.o: $(SRC)/sort.c $(SRC)/sort.h $(SRC)/cyclist.h
	$(CC) $(CFLAGS) -c $(SRC)/sort.c

stack.o: $(SRC)/stack.c $(SRC)/stack.h
	$(CC) $(CFLAGS) -c $(SRC)/stack.c

clear:
	$(RM) -r *.o && $(RM) -r ep2