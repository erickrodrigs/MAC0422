CC		     = gcc
CFLAGS     = -Wall -g
PTHREAD    = -lpthread
SRC		     = src
RM         = rm
#-----------------------

ep2: ep2.o
	$(CC) -o ep2 ep2.o $(PTHREAD)

ep2.o: $(SRC)/ep2.c
	$(CC) $(CFLAGS) -c $(SRC)/ep2.c

clear:
	$(RM) -r *.o && $(RM) -r ep2