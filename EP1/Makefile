CC		     = gcc
CFLAGS     = -Wall -g
READLINE   = -lreadline
PTHREAD    = -lpthread
SRC		     = src
RM         = rm
#-----------------------

bccsh: bccsh.o ep1
	$(CC) -o bccsh bccsh.o $(READLINE)

ep1: ep1.o heap.o queue.o
	$(CC) -o ep1 ep1.o heap.o queue.o $(PTHREAD)

bccsh.o: $(SRC)/bccsh.c
	$(CC) $(CFLAGS) -c $(SRC)/bccsh.c

ep1.o: $(SRC)/ep1.c $(SRC)/heap.h $(SRC)/queue.h
	$(CC) $(CFLAGS) -c $(SRC)/ep1.c

queue.o: $(SRC)/queue.c $(SRC)/queue.h $(SRC)/schedule.h
	$(CC) $(CFLAGS) -c $(SRC)/queue.c

heap.o: $(SRC)/heap.c $(SRC)/heap.h $(SRC)/schedule.h
	$(CC) $(CFLAGS) -c $(SRC)/heap.c

clear:
	$(RM) -r *.o && $(RM) -r bccsh ep1
