CC=g++
CPPFLAGS=-Wall
CPPFLAGS+=-lSDL -lSDLmain
CPPFLAGS+=-I/usr/include/SDL
SRC=*.cpp
OBJ=$(SRC: *.cpp=.o)
OUT=tanks

all: $(OUT)

$(SRC): *.h


$(OBJ):
	$(CC) $(SRC) $(CPPFLAGS) -c

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CPPFLAGS) -o $@
