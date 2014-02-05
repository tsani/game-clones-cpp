#include "Game.h"

#include "Application.h"

Game::Game(Application* a_owner)
{
    m_owner = a_owner;

    m_rdistribution = std::uniform_int_distribution<int>(0, 6); // there are 7 possible pieces

    for ( unsigned int i = 0; i < wellWidth; i++ )
        for ( unsigned int j = 0; j < wellHeight; j++ )
            m_well[i][j] = BlockState::free;
}

bool Game::load()
{
    // // Don't have resources yet.
    // m_blockSurfaces[BlockState::falling] = makeSafeSurfacePtr(loadOptimized("resources/falling-block.png"));
    // m_blockSurfaces[BlockState::fallen]  = makeSafeSurfacePtr(loadOptimized("resources/fallen-block.png"));
    
    Surface_ptr fallingSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF));
    Surface_ptr fallenSurface  = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF));
    SDL_FillRect(fallingSurface.get(), NULL, SDL_MapRGB(m_owner->getScreen().lock()->format, 255, 64, 64));
    SDL_FillRect(fallenSurface.get(), NULL, SDL_MapRGB(m_owner->getScreen().lock()->format, 64, 64, 255));

    m_blockSurfaces[BlockState::falling] = fallingSurface;
    m_blockSurfaces[BlockState::pivot]   = fallingSurface;
    m_blockSurfaces[BlockState::fallen]  = fallenSurface;

    spawnPiece(); // TESTING

    return true;
}

void Game::cleanup()
{

}

void Game::update()
{
    static unsigned long int frameNumber = 0;

    if ( frameNumber % 30 == 0 )
        updateBlocks();

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

}

void Game::draw(Surface_ptr a_parent)
{
    for ( unsigned short i = 0; i < wellWidth; i++)
    {
        for ( unsigned short j = 0; j < wellHeight; j++ )
        {
            if ( m_well[i][j] != BlockState::free )
            {
                SDL_Rect asdf { (short)(i * blockSide), (short)(j * blockSide), 0, 0 };
                SDL_BlitSurface(m_blockSurfaces[m_well[i][j]].get(), nullptr, a_parent.get(), &asdf);
            }
        }
    }
}

void Game::updateBlocks()
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
}

void Game::collideBlocks(unsigned int x, unsigned int y)
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
    if ( x - 1 <= 0 )
        collideBlocks(x - 1, y);
    if ( y - 1 <= 0 )
        collideBlocks(x, y - 1);
}

