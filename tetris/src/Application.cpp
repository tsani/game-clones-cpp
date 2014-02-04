#include "Application.h"

Application::Application()
    : m_state(APPSTATE_NOTREADY), m_score(0)
{
}

bool Application::load()
{
    m_state = APPSTATE_READY;
    return true;
}

int Application::run()
{
    if ( !load() )
    {
        std::cerr << "Loading failed. Exiting." << std::endl;
        return EXIT_FAILURE;
    }

    m_state = APPSTATE_RUNNING;

    Uint32 lastTime = 0, 
           thisTime = 0,
           deltaTime = 0,
           extraTime = 0;
    bool skipFrame = false;

    lastTime = SDL_GetTicks();

    update();

    while ( m_state == APPSTATE_RUNNING )
    {
        if ( !skipFrame )
            draw();

        thisTime = SDL_GetTicks(); // the current time
        deltaTime = thisTime - lastTime; // how long did it take to draw the last frame.
        extraTime = FRAMETIME - deltaTime; // how much time is "left" in the current frame.

        // true if the last frame took longer than 1/60 s to draw.
        skipFrame = deltaTime > FRAMETIME;

        // if there is sufficient time left over in this frame, simply stall the program for that time.
        if ( extraTime > 10 ) 
            SDL_Delay(extraTime);
        
        lastTime = thisTime; // update the last time to the current time. 
        update();
    }

    return EXIT_SUCCESS;
}
    

void Application::update()
{

}

void Application::draw()
{

}

