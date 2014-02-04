#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <deque>

enum ApplicationState
{
    APPSTATE_RUNNING,
    APPSTATE_FINISHED,
    APPSTATE_NOTREADY,
    APPSTATE_READY
};

class Application
{
    public:
        Application();

        /** The main loop of the program. The return value is a system-dependent program exit code.
         */
        int run();

        const unsigned int FRAMERATE = 60; // How many frames in one second?
        const double FRAMETIME = 1.0 / (double) FRAMERATE * 1000.0; // How long is one frame? (in ms)

    private:
        /** Update the game state. Returns false when the game should exit.
         */
        void update();

        /** Loads the initial game resources. Return values represents success.
         * This method will set the application state to APPSTATE_READY.
         */
        bool load();

        /** Draws the current game state to the screen.
         */
        void draw();

        /** Represents the current state of the application.
         */
        ApplicationState m_state;

        /** The current score of the player.
         */
        int m_score;
};
