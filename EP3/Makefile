CC		     = g++
CFLAGS     = -Wall -g
SRC		     = src
RM         = rm
#-----------------------

ep3: ep3.o disk.o diskoperations.o diskfile.o
	$(CC) -o ep3 ep3.o disk.o diskoperations.o diskfile.o

ep3.o: $(SRC)/ep3.cpp disk.o
	$(CC) $(CFLAGS) -c $(SRC)/ep3.cpp

disk.o: $(SRC)/disk.cpp $(SRC)/disk.hpp diskoperations.o
	$(CC) $(CFLAGS) -c $(SRC)/disk.cpp

diskoperations.o: $(SRC)/diskoperations.cpp $(SRC)/diskoperations.hpp diskfile.o
	$(CC) $(CFLAGS) -c $(SRC)/diskoperations.cpp

diskfile.o: $(SRC)/diskfile.cpp $(SRC)/diskfile.hpp $(SRC)/constants.hpp
	$(CC) $(CFLAGS) -c $(SRC)/diskfile.cpp

clear:
	$(RM) -r *.o ep3
