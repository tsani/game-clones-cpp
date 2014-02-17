#include "Application.h"

Application::Application()
    : State((const State*)nullptr), m_screen(nullptr)
{
    m_child = std::make_shared<MenuState>(this);
}

std::weak_ptr<SDL_Surface> Application::getScreen() const
{
    return std::weak_ptr<SDL_Surface>(m_screen);
}

void Application::load()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    m_screen =
        Screen_ptr
            (SDL_SetVideoMode(screenWidth, 
                              screenHeight, 
                              screenDepth, 
                              SDL_HWSURFACE | SDL_DOUBLEBUF),
             EmptyDeleter<SDL_Surface>());

    TTF_Init();

    State::load();
}

int Application::run()
{
    load ();

    activate ();

    Uint32 lastTime = 0, 
           thisTime = 0,
           deltaTime = 0,
           extraTime = 0;
    bool skipFrame = false;

    lastTime = SDL_GetTicks();

    m_child->activate();

    update ();

    while ( m_status == AppState::running )
    {
        if ( !skipFrame )
            draw (m_screen);
        //else
        //    std::cerr << "Skipped frame!" << std::endl;

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

    return EXIT_SUCCESS;
}

void Application::cleanup()
{
    State::cleanup();

    SDL_Quit();
}

void Application::update()
{
    static SDL_Event event;

    while ( SDL_PollEvent(&event) )
    {
        switch ( event.type )
        {
            case SDL_QUIT:
                std::cerr << "Got quit signal." << std::endl;
                cleanup();
                break;
            default:
                handleEvent(event); // this will pass the event on to the child
                break;
        }
    }

    State::update();

    if ( m_child == nullptr ) // if the child got nexted into null, then there is no more game
        cleanup(); // cleanup to finish this state, which will terminate the main loop
}

void Application::draw(Surface_ptr a_parent)
{
    static Uint32 black = SDL_MapRGB (m_screen->format, 0, 0, 0);
    SDL_FillRect (m_screen.get(), NULL, black); 
    State::draw (m_screen);
    SDL_Flip (m_screen.get());
}

