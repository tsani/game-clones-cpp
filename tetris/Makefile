.SUFFIXES:

BIN_NAME = tetris
CXX 	 = g++
LD       = g++
CXXFLAGS = -Wall -g -x c++ -fexceptions `sdl-config --cflags` -I include -std=c++11
LIBS     = `sdl-config --libs` -lSDL_image -lSDL_ttf
LDFLAGS  = $(LIBS)

all: obj/util_SDL.o obj/TetrisData.o obj/Application.o obj/Game.o obj/main.o
	@mkdir -p obj/
	@mkdir -p bin/
	$(LD) -o bin/$(BIN_NAME) obj/* $(LDFLAGS)

clean:
	rm -f obj/*

obj/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
