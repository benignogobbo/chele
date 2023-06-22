#
# To build the things
# Benigno Gobbo 
# 20200218
#

CC     = g++
CFLAGS = -O -std=c++11
#CFLAGS = -g -std=c++11

all: test reset dispon dispoff command updatesd

test  : test.cc cheleEthClient.h cheleEthClient.o
	$(CC) $(CFLAGS) -o test test.cc cheleEthClient.o

reset  : reset.cc cheleEthClient.h cheleEthClient.o
	$(CC) $(CFLAGS) -o reset reset.cc cheleEthClient.o

dispon  : dispon.cc cheleEthClient.h cheleEthClient.o
	$(CC) $(CFLAGS) -o dispon dispon.cc cheleEthClient.o

dispoff  : dispoff.cc cheleEthClient.h cheleEthClient.o
	$(CC) $(CFLAGS) -o dispoff dispoff.cc cheleEthClient.o

command  : command.cc cheleEthClient.h cheleEthClient.o
	$(CC) $(CFLAGS) -o command command.cc cheleEthClient.o

updatesd : updatesd.cc
	$(CC) $(CFLAGS) -o updatesd updatesd.cc 

cheleEthClient.o: cheleEthClient.cc cheleEthClient.h
	$(CC) $(CFLAGS) -c cheleEthClient.cc

clean: 
	rm -f *.o *.a *~ test reset dispon dispoff command updatesd

