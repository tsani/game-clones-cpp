#include "Game.h"

#include "Application.h"

#include <cmath>

Game::Game(Application* a_owner)
{
    m_owner = a_owner;
    m_speed = 30;

    m_rdistribution = std::uniform_int_distribution<int>(0, 6); // there are 7 possible pieces

    m_rotationLevel = 0;

    for ( unsigned int i = 0; i < wellWidth; i++ )
        for ( unsigned int j = 0; j < wellHeight; j++ )
            m_well[i][j] = BlockState::free;
}

bool Game::load()
{
    // // Don't have resources yet.
    // m_blockSurfaces[BlockState::falling] = makeSafeSurfacePtr(loadOptimized("resources/falling-block.png"));
    // m_blockSurfaces[BlockState::fallen]  = makeSafeSurfacePtr(loadOptimized("resources/fallen-block.png"));
    
    Surface_ptr fallingSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    Surface_ptr fallenSurface  = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    Surface_ptr freeSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    SDL_FillRect(fallingSurface.get(), NULL, SDL_MapRGB(m_owner->getScreen().lock()->format, 255, 64, 64));
    SDL_FillRect(fallenSurface.get(), NULL, SDL_MapRGB(m_owner->getScreen().lock()->format, 64, 64, 255));
    SDL_FillRect(freeSurface.get(), makeSafeRectPtr(8, 8, 16, 16).get(), SDL_MapRGB(m_owner->getScreen().lock()->format, 128, 128, 128));

    m_blockSurfaces[BlockState::falling] = std::move(fallingSurface);
    m_blockSurfaces[BlockState::pivot]   = std::move(fallingSurface);
    m_blockSurfaces[BlockState::fallen]  = std::move(fallenSurface);
    m_blockSurfaces[BlockState::free]    = std::move(freeSurface);

    // // for fancy input handling, which we don't need.
    // m_keyboard = SDL_GetKeyboardState();

    m_wellPosition.x = m_owner->screenWidth / 2 - wellWidth * blockSide / 2;
    m_wellPosition.y = m_owner->screenHeight / 2 - wellHeight * blockSide / 2;

    spawnPiece(); // initial piece

    return true;
}

void Game::cleanup()
{

}

void Game::update()
{
    static unsigned long int frameNumber = 0;

    if ( frameNumber % m_speed == 0 )
        updateBlocks();

    // // Handle input // lol i'm not in the mood for sophisticated input handling
    // if ( m_keyboard[SDL_SCANCODE_LEFT] )
    // {
    //     if ( m_keyTimes[SDL_SCANCODE_LEFT] == 0 )
    //         // perform left action
    //     else
    //         m_keyTimes[SDL_SCANCODE_LEFT]++;
    //     if ( m_keyTimes[SDL_SCANCODE_LEFT] % repeatThreshold )
    //         // perform left action
    // }
    // else
    //     m_keyTimes[SDL_SCANCODE_LEFT] = 0;

    frameNumber++;
}

void Game::spawnPiece()
{
    int newPieceType = m_rdistribution(m_rengine);
    
    for ( unsigned int i = 3; i < 8; i++ )
    {
        for ( unsigned int j = 0; j < 3; j++ )
        {
            if ( PIECES[newPieceType][0][j + 2][i - 3] != 0 )
                m_well[i][j] = PIECES[newPieceType][0][j + 2][i - 3] == 1 ? BlockState::falling : BlockState::pivot;
        }
    }

    std::cerr << "Spawned piece " << newPieceType << "." << std::endl;
}

void Game::handleEvent(SDL_Event const& event)
{
    switch ( event.type )
    {
        case SDL_KEYUP:
            switch ( event.key.keysym.sym )
            {
                case SDLK_LEFT:
                    movePiece(1, -1);
                    break;
                case SDLK_RIGHT:
                    movePiece(1, 1);
                    break;
                case SDLK_z:
                    rotateCCW();
                    break;
                case SDLK_x:
                    rotateCW();
                    break;
                case SDLK_SPACE:
                    fall();
                    break;
                default:
                    break;
            }
            break;
    }
}

void Game::draw(Surface_ptr a_parent)
{
    static SDL_Rect drawLocation { 0, 0, 0, 0 };

    for ( unsigned short i = 0; i < wellWidth; i++)
    {
        for ( unsigned short j = 0; j < wellHeight; j++ )
        {
            if ( m_well[i][j] != BlockState::free )
            {
                drawLocation.x = m_wellPosition.x + (short)(i * blockSide); 
                drawLocation.y = m_wellPosition.y + (short)(j * blockSide);
                if ( SDL_BlitSurface(m_blockSurfaces[m_well[i][j]].get(), nullptr, a_parent.get(), &drawLocation) != 0 )
                    std::cerr << "Failed to draw block at " << i << ", " << j << std::endl;
            }
        }
    }
}

bool Game::updateBlocks()
{
    std::vector<std::pair<unsigned int, unsigned int>> toShift;
    bool collided = false;

    for ( int i = 0; i < wellWidth; i++ )
    {
        for ( int j = wellHeight - 1; j >= 0; j-- )
        {
            if ( m_well[i][j] == BlockState::falling || m_well[i][j] == BlockState::pivot )
            {
                if ( j + 1 < wellHeight && m_well[i][j + 1] != BlockState::fallen)
                {
                    toShift.push_back(std::pair<unsigned int, unsigned int>(i, j));
                }
                else
                {
                    collideBlocks(i, j);
                    collided = true;
                    spawnPiece(); // spawn a new piece seeing as the current one is finished now.
                }
            }
        }
    }

    if ( !collided && toShift.size() > 0 )
    {
        for ( auto &point : toShift )
        {
            m_well[point.first][point.second + 1] = m_well[point.first][point.second];
            m_well[point.first][point.second] = BlockState::free;
            // std::cerr << "Block at " << point.first << ", " << point.second << " fell!" << std::endl;
        }
    }

    return collided;
}

void Game::collideBlocks(int x, int y)
{
    if ( m_well[x][y] == BlockState::falling || m_well[x][y] == BlockState::pivot )
    {
        m_well[x][y] = BlockState::fallen;
    }
    else
        return;

    if ( x + 1 < wellWidth )
        collideBlocks(x + 1, y);
    if ( y + 1 < wellHeight )
        collideBlocks(x, y + 1);
    if ( x - 1 >= 0 )
        collideBlocks(x - 1, y);
    if ( y - 1 >= 0 )
        collideBlocks(x, y - 1);
}

void Game::checkTetris()
{

}

void Game::rotateCW()
{

}

void Game::rotateCCW()
{

}

bool Game::movePiece(unsigned int dx, int direction, std::vector<std::pair<int, int>> & piece)
{
    int direction_ = std::abs(direction) / direction; // make sure it's just the sign
    if ( dx > 1 )
        if ( ! movePiece(dx - 1, direction, piece) )
            return false;

    std::map<std::pair<int, int>, BlockState> pieceData;

    for ( auto &p : piece ) // this loop checks that the movement is possible (will not collide with anything
    {
        if ( ! ( p.first + direction_ >= 0 && 
                 p.first + direction_ < wellWidth && 
                 (m_well[p.first + direction_][p.second] == BlockState::falling ||
                  m_well[p.first + direction_][p.second] == BlockState::pivot)
               )
           )
        {
            return false;
        }
        else
            pieceData[ { p.first + direction_, p.second } ] = m_well[p.first][p.second]; // save the information about the piece's state at the old position into the new position
    }


    for ( auto &p : piece ) // we delete the piece from the board
    {
        m_well[p.first][p.second] = BlockState::free;
        p.first += direction_; // and we shift over this block
    }

    for ( auto &p : piece ) // we recreate the piece at the old location
    {
        m_well[p.first][p.second] = pieceData[p];
    }

    return true;
}

void Game::fall()
{
    while ( ! updateBlocks() );
}

SDL_Rect Game::findPivot()
{
    for ( short i = 0; i < wellWidth; i++ )
    {
        for ( short j = 0; j < wellHeight; j++ )
        {
            if ( m_well[i][j] == BlockState::pivot )
                return makeRect(i, j, 0, 0);
        }
    }

    throw std::exception();
}

std::vector<std::pair<int, int>> Game::findPiece()
{
    SDL_Rect pivot = findPivot();
    std::vector<std::pair<int, int>> piece;

    short xstart = pivot.x - 2 < 0 ? 0 : pivot.x - 2;
    short xend   = pivot.x + 2 >= wellWidth ? wellWidth : pivot.x + 3;
    short ystart = pivot.y - 2 < 0 ? 0 : pivot.y - 2;
    short yend   = pivot.y + 2 >= wellHeight ? wellHeight : pivot.y + 3;

    for ( short i = xstart; i < xend; i++ )
    {
        for ( short j = ystart; j < yend; j++ )
        {
            if ( m_well[i][j] == BlockState::falling || m_well[i][j] == BlockState::pivot )
                piece.push_back( { i, j } );
        }
    }

    return piece;
}


