#include "Game.h"
#include "Application.h"

Game::Game(const Application* a_owner, unsigned int a_initialSpeed)
    : State (a_owner), m_well(wellWidth, wellHeight)
{
    m_speed = a_initialSpeed;
    m_score = 0;
    m_clearedLines = 0;

    m_fallFaster = false;
    m_falling = true;
    m_easyMode = false;

    m_child = State_ptr { new MultiState(this) };

    m_time = 1;
}

void Game::load()
{
    // // Don't have resources yet.
    // m_blockSurfaces[BlockState::falling] = makeSafeSurfacePtr(loadOptimized("resources/falling-block.png"));
    // m_blockSurfaces[BlockState::fallen]  = makeSafeSurfacePtr(loadOptimized("resources/fallen-block.png"));
    
    m_pieceSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    m_fallenSurface  = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    m_freeSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    m_clearedSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide * m_well.getWellWidth(), blockSide, Application::screenDepth, 0, 0, 0, 0));
    m_fallenPreviewSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    SDL_FillRect(m_pieceSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 255, 64, 64));
    SDL_FillRect(m_fallenSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 64, 64, 255));
    SDL_FillRect(m_freeSurface.get(), makeSafeRectPtr(8, 8, 16, 16).get(), SDL_MapRGB(getOwner()->getScreen().lock()->format, 128, 128, 128));
    SDL_FillRect(m_clearedSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 255, 255, 255));
    SDL_FillRect(m_fallenPreviewSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 64, 255, 64));

    m_wellPosition.x = getOwner()->screenWidth / 2 - m_well.getWellWidth() * blockSide / 2;
    m_wellPosition.y = getOwner()->screenHeight / 2 - m_well.getWellHeight() * blockSide / 2;

    m_piecePreviewPosition.x = m_wellPosition.x + (m_well.getWellWidth() + 2) * blockSide;
    m_piecePreviewPosition.y = m_wellPosition.y + 8 * blockSide;

    m_statusLocation.x = 20;
    m_statusLocation.y = 100;

    m_statusFont    = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 20));

    m_statusColorFgNormal = { 255, 255, 255 };
    m_statusColorFgEffect = { 255, 64, 64 };
    m_statusColorBg = { 0, 0, 0 };

    renderStatus(&m_scoreSurface, "Score: ", m_score, m_statusColorFgNormal);
    renderStatus(&m_levelSurface, "Level: ", m_speed, m_statusColorFgNormal);
    renderStatus(&m_linesSurface, "Lines: ", m_clearedLines, m_statusColorFgNormal);

    if ( ! m_well.newPiece() )
        std::cerr << "Failed to spawn initial piece." << std::endl;

    m_child->activate();

    State::load();
}

void Game::cleanup()
{
    State::cleanup();
}

void Game::update()
{
    // Make the blocks fall only once every (speedLimit / m_speed) frames
    if ( m_time % (speedLimit / (m_fallFaster ? 10 > m_speed ? 10 : m_speed : m_speed)) == 0 )
        if ( m_falling && m_well.updatePiece() ) // if a collision took place
            handleNewPiece();

    State::update();

    m_time++;
}

void Game::handleEvent(SDL_Event const& event)
{
    switch ( event.type )
    {
        case SDL_KEYUP:
            switch ( event.key.keysym.sym )
            {
                case SDLK_g:
                    m_easyMode = !m_easyMode;
                    break;
                case SDLK_DOWN:
                    m_fallFaster = false;
                    break;
                default:
                    break;
            }

            if ( m_falling ) // all this input only makes sense if a piece is falling.
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_LEFT:
                        m_well.movePiece(-1);
                        // std::cerr << "Piece moved left: " << status << std::endl;
                        break;
                    case SDLK_RIGHT:
                        m_well.movePiece(1);
                        // std::cerr << "Piece moved right!" << status << std::endl;
                        break;
                    case SDLK_z:
                        m_well.rotatePiece(Direction::CCW);
                        break;
                    case SDLK_x:
                        m_well.rotatePiece(Direction::CW);
                        break;
                    case SDLK_SPACE:
                        m_well.fall();
                        handleNewPiece();
                        break;
                    default:
                        break;
                }
            }
            break;
        case SDL_KEYDOWN:
            if ( event.key.keysym.sym == SDLK_DOWN )
                m_fallFaster = true;
            break;
    }
}

void Game::draw(Surface_ptr a_parent)
{
    static SDL_Rect drawLocation { 0, 0, 0, 0 };

    for ( unsigned short i = 0; i < m_well.getWellWidth(); i++)
    {
        for ( unsigned short j = 0; j < m_well.getWellHeight(); j++ )
        {
            drawLocation.x = m_wellPosition.x + (short)(i * blockSide); 
            drawLocation.y = m_wellPosition.y + (short)(j * blockSide);

            Surface_ptr surface = m_well.getWell()[i][j] ? m_fallenSurface : m_freeSurface;

            if ( SDL_BlitSurface(surface.get(), nullptr, a_parent.get(), &drawLocation) != 0 )
                std::cerr << "Failed to draw well surface." << std::endl;
        }
    }

    auto drawPiece = [this, a_parent] (Well::Piece const& piece, Surface_ptr pieceSurface)
    {
        SDL_Rect drawLocation { 0, 0, 0, 0 };

        for ( auto &p : piece )
        {
            drawLocation.x = m_wellPosition.x + (short)(p.location.first * blockSide);
            drawLocation.y = m_wellPosition.y + (short)(p.location.second * blockSide);

            if ( SDL_BlitSurface(pieceSurface.get(), nullptr, a_parent.get(), &drawLocation) != 0 )
                std::cerr << "Failed to draw piece surface." << std::endl;
        }
    };

    drawPiece(m_well.getPiece(), m_pieceSurface);

    if ( m_falling && m_easyMode )
        drawPiece(m_well.getFallenPiece(), m_fallenPreviewSurface);

    for ( auto &p : m_clearingSurfaces )
    {
        if ( SDL_BlitSurface(m_clearedSurface.get(), nullptr, a_parent.get(), &p) != 0 )
            std::cerr << "Failed to draw clearing surface." << std::endl;
    }

    drawLocation = m_statusLocation;
    if ( SDL_BlitSurface(m_scoreSurface.get(), nullptr, a_parent.get(), &drawLocation) 
            != 0 )
        std::cerr << "Failed to draw score surface." << std::endl;

    drawLocation.y += 3 * m_scoreSurface->h / 2;
    if ( SDL_BlitSurface(m_levelSurface.get(), nullptr, a_parent.get(), &drawLocation)
            != 0 )
        std::cerr << "Failed to draw level surface." << std::endl;

    drawLocation.y += 3 * m_levelSurface->h / 2;
    if ( SDL_BlitSurface(m_linesSurface.get(), nullptr, a_parent.get(), &drawLocation)
            != 0 )
        std::cerr << "Failed to draw lines surface." << std::endl;

    drawPreviewBox(a_parent);
}

void Game::drawPreviewBox(Surface_ptr a_parent)
{
    static SDL_Rect drawLocation { 0, 0, 0, 0 };

    for ( int i = 0; i < 5; i++ )
    {   
        drawLocation.x = m_piecePreviewPosition.x + i * blockSide;

        for ( int j = 0; j < 5; j++ )
        {
            drawLocation.y = m_piecePreviewPosition.y = j * blockSide;
            if( SDL_BlitSurface((PIECES[m_well.getNextPieceID()][0][j][i] == 0 ? m_freeSurface : m_pieceSurface).get(), 
                        nullptr, a_parent.get(), &drawLocation) != 0 )
                std::cerr << "Failed to draw preview block at " << i << ", " << j << std::endl;
        }
    }
                                  
}

void Game::renderStatus(Surface_ptr * dest, std::string const& text, unsigned int value, SDL_Color fg)
{
    static std::stringstream sb;

    sb.clear();
    sb.str("");
    sb << text << value;
    (*dest) = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_statusFont.get(), sb.str().c_str(), fg, m_statusColorBg));
}

void Game::renderStatusWithEffect(Surface_ptr * dest, std::string const& text, unsigned int value, SDL_Color fg1, SDL_Color fg2, unsigned int delayTime)
{
    auto f = std::bind(&Game::renderStatus, this, dest, text, value, std::placeholders::_1);
    f(fg1);
    getChild()->add(makeDelay(this, delayTime, std::bind(f, fg2)));
}

void Game::renderStatusWithDefaultEffect(Surface_ptr * dest, std::string const& text, unsigned int value)
{
    renderStatusWithEffect(dest, text, value, m_statusColorFgEffect, m_statusColorFgNormal, statusChangeEffectTime);
}

void Game::handleSpeed()
{

    // +1 to account for the fact that with less than speedStep lines cleared, the division yields zero.
    unsigned int ts = m_clearedLines / speedStep + 1;
    if ( m_speed < ts )
    {
        m_speed = ts;
        std::cerr << "Speed up! " << ts << std::endl;
        renderStatusWithDefaultEffect(&m_levelSurface, "Level: ", m_speed);
    }
}

void Game::handleGenNewPiece()
{
    if ( ! m_well.newPiece() )
    {
        setState(AppState::finished);
        m_next = State_ptr { new GameOverState(getOwner(), m_score, m_speed, m_clearedLines) };
    }
    else
    {
        m_time = 1;
    }

    m_falling = true;
}

void Game::handleRows()
{
    auto rows = m_well.getFullRows();

    if ( rows.size() > 0)
    {  
        auto callback_f = [this, rows] () {
            handleGenNewPiece();
            m_clearingSurfaces.clear();
            m_score += baseRowScore * rows.size() * std::pow(2, rows.size() - 1); // TODO bells!!!
            m_clearedLines += rows.size();
            m_well.removeRows(std::move(rows));
            handleSpeed();
            renderStatusWithDefaultEffect(&m_scoreSurface, "Score: ", m_score);
            renderStatusWithDefaultEffect(&m_linesSurface, "Lines: ", m_clearedLines);
        };

        for ( auto i = rows.begin(); i != rows.end(); i++ )
             m_clearingSurfaces.push_back(SDL_Rect { m_wellPosition.x, (short)(m_wellPosition.y + *i * blockSide), 0, 0 } );

        getChild()->add(makeDelay(this, 30, callback_f));
    }
    else
        handleGenNewPiece();
}

void Game::handleNewPiece()
{
    auto f = std::bind(&Game::handleRows, this);
    getChild()->add(makeDelay(this, 15, f));

    m_falling = false; // disable the falling piece during the forstate's execution for the effect
}


