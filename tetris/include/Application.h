#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>

#include "util_SDL.h"

#include "Game.h"

enum class AppState
{
    running,
    finished,
    notReady,
    ready
};

class Application
{
    public:
        Application();

        /** The main loop of the program. The return value is a system-dependent program exit code.
         */
        int run();

        std::weak_ptr<SDL_Surface> getScreen();

        static const unsigned int screenHeight = 700;
        static const unsigned int screenWidth  = 950;
        static const unsigned int screenDepth  = 32;

        static const unsigned int FRAMERATE = 60; // How many frames in one second?
        static const unsigned int FRAMETIME = 1000 / FRAMERATE; // How long is one frame? (in ms)

    private:
        /** Update the game state. Returns false when the game should exit.
         */
        void update();

        /** Loads the initial game resources. Return values represents success.
         * This method will set the application state to APPSTATE_READY.
         */
        bool load();

        /** Cleans up resources and calls SDL_Quit.
         */
        void cleanup();

        /** Draws the current game state to the screen.
         */
        void draw();

        /** Represents the current state of the application.
         */
        AppState m_state;

        Screen_ptr m_screen;

        Game m_game;
};

#endif
