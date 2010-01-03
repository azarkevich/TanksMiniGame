CC=g++
CPPFLAGS=-Wall
CPPFLAGS+=-lSDL -lSDLmain
CPPFLAGS+=-I/usr/include/SDL
SRC=*.cpp
HDR=*.h
OBJ=$(SRC: *.cpp=.o)
OUT=tanks

all: $(OUT)

$(OBJ): $(HDR)
	$(CC) $(SRC) $(CPPFLAGS) -c

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CPPFLAGS) -o $@
