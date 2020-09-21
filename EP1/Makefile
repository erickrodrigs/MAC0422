CC		 = gcc
CFLAGS = -Wall -g
LIBS   = -lreadline
SRC		 = src
RM     = rm
#-----------------------

bccsh: bccsh.o
	$(CC) -o bccsh bccsh.o $(LIBS)

bccsh.o: $(SRC)/bccsh.c
	$(CC) $(CFLAGS) -c $(SRC)/bccsh.c

clear:
	$(RM) -r *.o && $(RM) -r bccsh
