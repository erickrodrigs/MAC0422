CC		     = gcc
CFLAGS     = -Wall -g
READLINE   = -lreadline
PTHREAD    = -lpthread
SRC		     = src
RM         = rm
#-----------------------

bccsh: bccsh.o ep1
	$(CC) -o bccsh bccsh.o $(READLINE)

ep1: ep1.o
	$(CC) -o ep1 ep1.o $(PTHREAD)

bccsh.o: $(SRC)/bccsh.c
	$(CC) $(CFLAGS) -c $(SRC)/bccsh.c

ep1.o: $(SRC)/ep1.c
	$(CC) $(CFLAGS) -c $(SRC)/ep1.c

clear:
	$(RM) -r *.o && $(RM) -r bccsh ep1
