CC = g++
PROJECT = start
SRC = main.cpp
LIBS = `pkg-config --cflags --libs opencv4`
VCLIB = vc.c
VCCC = gcc
MYLIB = vc.o
$(PROJECT) : $(SRC)
		$(VCCC) -c $(VCLIB)
		$(CC) $(SRC) $(MYLIB) -o $(PROJECT) $(LIBS)