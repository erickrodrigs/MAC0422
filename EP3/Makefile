CC		     = g++
CFLAGS     = -Wall -g
SRC		     = src
RM         = rm
#-----------------------

ep3: ep3.o
	$(CC) -o ep3 ep3.o

ep3.o: $(SRC)/ep3.cpp
	$(CC) $(CFLAGS) -c $(SRC)/ep3.cpp

clear:
	$(RM) -r *.o ep3