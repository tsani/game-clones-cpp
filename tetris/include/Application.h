#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <deque>

#include "util_SDL.h"

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

        struct
        {
            const unsigned int height = 600;
            const unsigned int width  = 800;
            const unsigned int depth  = 32;
        } screen;

        const unsigned int FRAMERATE = 60; // How many frames in one second?
        const unsigned int FRAMETIME = 1000 / FRAMERATE; // How long is one frame? (in ms)

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

        /** The current score of the player.
         */
        int m_score;

        Screen_ptr m_screen;

};

#endif
