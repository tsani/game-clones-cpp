#include "Application.h"

Application::Application()
    : m_state(AppState::notReady), m_game(this)
{
}

std::weak_ptr<SDL_Surface> Application::getScreen()
{
    return std::weak_ptr<SDL_Surface>(m_screen);
}

bool Application::load()
{
    m_state = AppState::ready;
    SDL_Init(SDL_INIT_EVERYTHING);
    m_screen =
        Screen_ptr
            (SDL_SetVideoMode(screenWidth, 
                              screenHeight, 
                              screenDepth, 
                              SDL_HWSURFACE | SDL_DOUBLEBUF),
             EmptyDeleter<SDL_Surface>());

    TTF_Init();

    return m_game.load();
}

int Application::run()
{
    if ( !load() )
    {
        std::cerr << "Loading failed. Exiting." << std::endl;
        return EXIT_FAILURE;
    }

    m_state = AppState::running;

    Uint32 lastTime = 0, 
           thisTime = 0,
           deltaTime = 0,
           extraTime = 0;
    bool skipFrame = false;

    lastTime = SDL_GetTicks();

    update();

    while ( m_state == AppState::running )
    {
        if ( !skipFrame )
            draw();
        else
            std::cerr << "Skipped frame!" << std::endl;

        thisTime = SDL_GetTicks(); // the current time
        deltaTime = thisTime - lastTime; // how long did it take to draw the last frame.
        // std::cerr << "This time = " << thisTime << " ; last time = " << lastTime << std::endl;
        extraTime = deltaTime < FRAMETIME ? FRAMETIME - deltaTime : 0; // how much time is "left" in the current frame.
        
        // std::cerr << "Extra time: " << extraTime << std::endl;

        // true if the last frame took longer than 1/60 s to draw.
        skipFrame = deltaTime > FRAMETIME;

        // if there is sufficient time left over in this frame, simply stall the program for that time.
        if ( extraTime > 10 ) 
            SDL_Delay(extraTime);
        
        lastTime = SDL_GetTicks(); // update the last time to the current time. 

        update();
    }

    cleanup();

    return EXIT_SUCCESS;
}

void Application::cleanup()
{
    m_game.cleanup();
    SDL_Quit();
}

void Application::update()
{
    static SDL_Event event;

    m_game.update();

    while ( SDL_PollEvent(&event) )
    {
        switch ( event.type )
        {
            case SDL_QUIT:
                std::cerr << "Got quit signal." << std::endl;
                m_state = AppState::finished;
                break;
            default:
                m_game.handleEvent(event);
                break;
        }
    }
}

void Application::draw()
{
    static Uint32 black = SDL_MapRGB(m_screen->format, 0, 0, 0);
    SDL_FillRect(m_screen.get(), NULL, black); 
    m_game.draw(m_screen);
    SDL_Flip(m_screen.get());
}

