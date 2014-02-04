.SUFFIXES:

BIN_NAME = tetris
CXX 	 = g++
LD       = g++
CXXFLAGS = -Wall -g -x c++ -fexceptions `sdl-config --cflags` -I include -std=c++11
LIBS     = `sdl-config --libs` -lSDL_image
LDFLAGS  = $(LIBS)

all: obj/Application.o obj/main.o
	@mkdir -p obj/
	@mkdir -p bin/
	$(LD) -o bin/$(BIN_NAME) obj/* $(LDFLAGS)

clean:
	rm -f obj/*

obj/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
