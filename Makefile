CC = g++
PROJECT = start
SRC = main.cpp
LIBS = `pkg-config --cflags --libs opencv4`
MYLIB = vc.o
$(PROJECT) : $(SRC)
		$(CC) $(SRC) $(MYLIB) -o $(PROJECT) $(LIBS)