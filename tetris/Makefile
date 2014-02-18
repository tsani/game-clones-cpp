.SUFFIXES:

BIN_NAME = "tetris"

CXX 	 = $(CROSS)g++
LD       = $(CROSS)g++
CXXFLAGS = -Wall -Werror -g -x c++ -fexceptions `$(CROSS)pkg-config --cflags sdl SDL_image SDL_ttf` -iquote include -std=c++11
LIBS     = `$(CROSS)pkg-config --libs sdl SDL_image SDL_ttf` 
LDFLAGS  = -Wl,-Bdynamic $(LIBS)

all: obj/util_SDL.o obj/State.o obj/TetrisData.o obj/Application.o obj/Game.o obj/GameOverState.o obj/MenuState.o obj/Well.o obj/main.o
	@mkdir -p obj/
	@mkdir -p bin/
	$(LD) -o bin/$(BIN_NAME) obj/* $(LDFLAGS)

clean:
	rm -f obj/*

obj/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

pkg-win:
	mv bin/tetris bin/tetris.exe
	mv bin/ tetris/
	zip -r tetris.zip tetris/
	mv tetris/ bin/
	mv tetris.zip dist/

pkg-nix:
	mv bin/ tetris/
	tar cf tetris.tar.gz tetris/
	mv tetris.tar.gz dist/
	mv tetris/ bin/
