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

    m_blockSurfaces[BlockState::falling] = fallingSurface;
    m_blockSurfaces[BlockState::pivot]   = (fallingSurface);
    m_blockSurfaces[BlockState::fallen]  = (fallenSurface);
    m_blockSurfaces[BlockState::free]    = (freeSurface);

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

void Game::removeFallingPiece()
{
    for ( auto &p : findPiece() )
    {
        m_well[p.first][p.second] = BlockState::free;
    }
}

bool Game::spawnPiece(int x, int y, int pieceID, int rotationID)
{
    pieceID == -1 ? pieceID = m_rdistribution(m_rengine) : pieceID;

    if ( pieceID < 0 || pieceID > 6 || rotationID < 0 || rotationID > 3 )
        throw std::exception(); // TODO make this more descriptive.

    std::map<std::pair<int, int>, BlockState> pieceData;
    
    for ( int i = 0; i < 5; i++ )
    {
        for ( int j = 0; j < 5; j++ )
        {
            if ( PIECES[pieceID][rotationID][j][i] != 0 )
            {
                int px = x - 2 + i, py = y - 2 + j; // where in the well block piece[j + 2][i] is going.
                // This check guarantees that the new block of the piece is in the well and not colliding
                if ( px < wellWidth && px >= 0 && py < wellHeight && py >= 0 && m_well[px][py] != BlockState::fallen)
                    pieceData[{px, py}] = PIECES[pieceID][rotationID][j][i] == 1 ? BlockState::falling : BlockState::pivot;
                else
                    return false;
            }
        }
    }

    // if we make it here, it's that the block is safe to place.

    removeFallingPiece(); // we remove the currently falling piece to avoid there being two falling pieces at once.
    for ( auto &p : pieceData )
    {
        m_well[p.first.first][p.first.second] = p.second;
    }

    m_pieceID = pieceID;
    m_rotationLevel = rotationID;

    std::cerr << "Spawned piece " << pieceID << "." << std::endl;

    return true;
}

void Game::handleEvent(SDL_Event const& event)
{
    switch ( event.type )
    {
        case SDL_KEYUP:
            switch ( event.key.keysym.sym )
            {
                case SDLK_LEFT:
                    std::cerr << "Piece moved left: " << movePiece(1, -1) << std::endl;
                    break;
                case SDLK_RIGHT:
                    std::cerr << "Piece moved right!" << movePiece(1, 1) << std::endl;
                    break;
                case SDLK_z:
                    rotate(1);
                    break;
                case SDLK_x:
                    rotate(-1);
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
            drawLocation.x = m_wellPosition.x + (short)(i * blockSide); 
            drawLocation.y = m_wellPosition.y + (short)(j * blockSide);
            if ( SDL_BlitSurface(m_blockSurfaces[m_well[i][j]].get(), nullptr, a_parent.get(), &drawLocation) != 0 )
                std::cerr << "Failed to draw block at " << i << ", " << j << std::endl;
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
                    checkTetris();
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
    for ( int y = 0; y < wellHeight; y++ )
    {
        int x = 0;
        for ( x = 0; x < wellWidth; x++ )
        {
            if ( m_well[x][y] != BlockState::fallen )
                break;
        }

        if ( x == wellWidth ) // TETRIS !! // TODO something with bells and whistles
        {
            for ( int y_ = y; y_ > 0; y_-- ) // we don't want this loop to touch the top row
            {
                for ( x = 0; x < wellWidth; x++ ) // turns out we can reuse our x variable.
                    m_well[x][y_] = m_well[x][y_ - 1]; // shift down
            }

            for ( x = 0; x < wellWidth; x++ )
                m_well[x][0] = BlockState::free; // simply erase the top row
        }
    }
}

bool Game::rotate(int direction)
{
    auto p = findPivot();
    auto direction_ = std::abs(direction) / direction;
    int newRotation;

    if ( m_rotationLevel + direction_ < 0 )
        newRotation = 3;
    else if ( m_rotationLevel + direction_ > 3 )
        newRotation = 0;
    else
        newRotation = m_rotationLevel + direction_;

    return spawnPiece(p.x, p.y, m_pieceID, newRotation);

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
                 m_well[p.first + direction_][p.second] != BlockState::fallen
               )
           )
        {
            std::cerr << "Piece cannot be moved due to collision." << std::endl;
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
    SDL_Rect pivot; 
    
    try
    {
        pivot = findPivot();
    }
    catch(std::exception)
    {
        return std::vector<std::pair<int, int>>();
    }

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



