#include "Game.h"
#include "Application.h"

Game::Game(Application* a_owner)
    : State (a_owner)
{
    m_speed = 3;

    m_rdistribution = std::uniform_int_distribution<int>(0, 6); // there are 7 possible pieces

    m_rotationLevel = 0;

    m_fallFaster = false;

    for ( unsigned int i = 0; i < wellWidth; i++ )
        for ( unsigned int j = 0; j < wellHeight; j++ )
            m_well[i][j] = BlockState::free;
}

void Game::load()
{
    // // Don't have resources yet.
    // m_blockSurfaces[BlockState::falling] = makeSafeSurfacePtr(loadOptimized("resources/falling-block.png"));
    // m_blockSurfaces[BlockState::fallen]  = makeSafeSurfacePtr(loadOptimized("resources/fallen-block.png"));
    
    Surface_ptr fallingSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    Surface_ptr fallenSurface  = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    Surface_ptr freeSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, blockSide, blockSide, Application::screenDepth, 0, 0, 0, 0));
    SDL_FillRect(fallingSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 255, 64, 64));
    SDL_FillRect(fallenSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 64, 64, 255));
    SDL_FillRect(freeSurface.get(), makeSafeRectPtr(8, 8, 16, 16).get(), SDL_MapRGB(getOwner()->getScreen().lock()->format, 128, 128, 128));

    m_blockSurfaces[BlockState::falling] = fallingSurface;
    m_blockSurfaces[BlockState::pivot]   = fallingSurface;
    m_blockSurfaces[BlockState::fallen]  = fallenSurface;
    m_blockSurfaces[BlockState::free]    = freeSurface;

    m_wellPosition.x = getOwner()->screenWidth / 2 - wellWidth * blockSide / 2;
    m_wellPosition.y = getOwner()->screenHeight / 2 - wellHeight * blockSide / 2;

    m_piecePreviewPosition.x = m_wellPosition.x + (wellWidth + 2) * blockSide;
    m_piecePreviewPosition.y = m_wellPosition.y + 2 * blockSide;

    m_statusLocation.x = 20;
    m_statusLocation.y = 100;

    m_statusFont    = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 20));

    m_statusColorFg = { 255, 255, 255 };
    m_statusColorBg = { 0, 0, 0 };

    renderScore();
    renderLevel();

    m_nextPieceID = m_rdistribution(m_rengine);
    newPiece();

    State::load();
}

void Game::cleanup()
{
    State::cleanup();
}

void Game::update()
{
    static unsigned long int frameNumber = 0;

    try
    {
        // Make the blocks fall only once every (speedLimit / m_speed) frames
        if ( frameNumber % (speedLimit / (m_fallFaster ? 10 > m_speed ? 10 : m_speed : m_speed)) == 0 )
            updateBlocks();
    }
    catch (GameOverException)
    {
        setState(AppState::finished);
        m_next = std::shared_ptr<State>(new GameOverState(getOwner(), m_score, m_speed));
    }

    State::update();

    frameNumber++;
}

void Game::removeFallingPiece()
{
    for ( auto &p : findPiece() )
    {
        m_well[p.first][p.second] = BlockState::free;
    }
}

bool Game::spawnPiece(int x, int y, unsigned int pieceID, int rotationID)
{
    if ( pieceID > 6 || rotationID < 0 || rotationID > 3 )
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

    // place the new piece
    for ( auto &p : pieceData )
    {
        m_well[p.first.first][p.first.second] = p.second;
    }

    // guarantee that m_pieceID and m_rotationLevel matche the values we were given.
    m_pieceID = pieceID;
    m_rotationLevel = rotationID;

    // diagnostic message
    // std::cerr << "Spawned piece " << pieceID << "." << std::endl;

    return true;
}

void Game::newPiece()
{
    if ( ! spawnPiece(pieceStartX, pieceStartY, m_nextPieceID) )
        throw GameOverException();

    m_nextPieceID = m_rdistribution(m_rengine);
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
                    // std::cerr << "Piece moved left: " << status << std::endl;
                    break;
                case SDLK_RIGHT:
                    movePiece(1, 1);
                    // std::cerr << "Piece moved right!" << status << std::endl;
                    break;
                case SDLK_DOWN:
                    m_fallFaster = false;
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
        case SDL_KEYDOWN:
            if ( event.key.keysym.sym == SDLK_DOWN )
                m_fallFaster = true;
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
            if ( SDL_BlitSurface(m_blockSurfaces[m_well[i][j]].get(), 
                                   nullptr, 
                                   a_parent.get(), 
                                   &drawLocation) != 0 )
                std::cerr << "Failed to draw block at " << i << ", " << j << std::endl;
        }
    }

    drawLocation = m_statusLocation;
    if ( SDL_BlitSurface(m_scoreSurface.get(), nullptr, a_parent.get(), &drawLocation) 
            != 0 )
        std::cerr << "Failed to draw score surface." << std::endl;

    drawLocation.y += 3 * m_scoreSurface->h / 2;
    if ( SDL_BlitSurface(m_levelSurface.get(), nullptr, a_parent.get(), &drawLocation)
            != 0 )
        std::cerr << "Failed to draw level surface." << std::endl;

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
            if( SDL_BlitSurface(
                        (m_blockSurfaces[PIECES[m_nextPieceID][0][j][i] == 0 ? // I am not a smart man.
                            BlockState::free : BlockState::falling].get()),
                        nullptr,
                        a_parent.get(),
                        &drawLocation) != 0 )
                std::cerr << "Failed to draw previed block at " << i << ", " << j << std::endl;
        }
    }
                                  
}

bool Game::updateBlocks()
{
    std::map<std::pair<unsigned int, unsigned int>, BlockState> toShift;
    std::vector<std::pair<unsigned int, unsigned int>> currentPiece;
    bool collided = false;

    currentPiece = findPiece();

    for ( auto &p : currentPiece )
    {
        if ( p.second + 1 < wellHeight && m_well[p.first][p.second + 1] != BlockState::fallen )
        {
            toShift[p] = m_well[p.first][p.second];
        }
        else
        {
            for ( auto &q : currentPiece )
                m_well[q.first][q.second] = BlockState::fallen;
            handleRows();
            collided = true;
            newPiece();
            break;
        }
    }

    if ( !collided )
    {
        if ( toShift.size() > 0 )
        {
            removeFallingPiece();
            for ( auto &point : toShift )
            {
                m_well[point.first.first][point.first.second + 1] = point.second;
                // std::cerr << "Block at " << point.first << ", " << point.second << " fell!" << std::endl;
            }
        }
        else // very special case that occurs when the game is lost by falling
        {
            return true;
        }
    }

    return collided;
}

void Game::removeRows(std::vector<int> && a_rows)
{
    for ( auto &row : a_rows )
    {
        for ( int y = row; y > 0; y-- )
        {
            for ( int x = 0; x < wellWidth; x++ )
                m_well[x][y] = m_well[x][y - 1];
        }
    }

    for ( int x = 0; x < wellWidth; x++ )
        m_well[x][0] = BlockState::free;
}

std::vector<int> Game::getFullRows()
{
    std::vector<int> rows;

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
            rows.push_back(y);
            // NOTE there may be a glitch here if the player fills the top row, although afaict, that is impossible (the player would lose).
        }
    }

    return rows;
}

void Game::renderLevel()
{
    static std::stringstream sb;

    sb.clear();
    sb.str("");
    sb << "Speed: " << m_speed;
    m_levelSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_statusFont.get(), sb.str().c_str(), m_statusColorFg, m_statusColorBg));
}

void Game::renderScore()
{
    static std::stringstream sb;

    sb.clear();
    sb.str("");
    sb << "Score: " << m_score;
    m_scoreSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_statusFont.get(), sb.str().c_str(), m_statusColorFg, m_statusColorBg));
}

void Game::handleSpeed()
{

    // +1 to account for the fact that with less than speedStep lines cleared, the division yields zero.
    unsigned int ts = m_clearedLines / speedStep + 1;
    if ( m_speed < ts )
    {
        m_speed = ts;
        // std::cerr << "Speed up! " << ts << std::endl;
        renderLevel();
    }
}

void Game::handleRows()
{
    std::vector<int> rows = getFullRows();

    if ( rows.size() > 0)
    {  
        m_score += baseRowScore * std::pow(rows.size(), rows.size()); // TODO bells!!!
        m_clearedLines += rows.size();
        removeRows(std::move(rows));
        handleSpeed();
        renderScore();;
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

    // spawnPiece will take care of removing the old piece from the well, and will effectively replace it by its rotated version.
    return spawnPiece(p.x, p.y, m_pieceID, newRotation);

}

bool Game::movePiece(unsigned int dx, int direction, std::vector<std::pair<unsigned int, unsigned int>> & piece)
{
    int direction_ = std::abs(direction) / direction; // make sure it's just the sign
    if ( dx > 1 )
        if ( ! movePiece(dx - 1, direction, piece) )
            return false;

    std::map<std::pair<unsigned int, unsigned int>, BlockState> pieceData;

    for ( auto &p : piece ) // this loop checks that the movement is possible (will not collide with anything
    {
        if ( ! ( !(p.first == 0 && direction_ < 0) && 
                 p.first + direction_ < wellWidth && 
                 m_well[p.first + direction_][p.second] != BlockState::fallen
               )
           )
        {
            // std::cerr << "Piece cannot be moved due to collision." << std::endl;
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

    for ( auto &p : piece ) // we recreate the piece at the new location
    {
        m_well[p.first][p.second] = pieceData[p];
    }

    return true;
}

void Game::fall()
{
    try
    {
        while ( ! updateBlocks() );
    }
    catch (GameOverException)
    {
        setState(AppState::finished);
        m_next = std::shared_ptr<State>(new GameOverState(getOwner(), m_score, m_speed));
    }
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

std::vector<std::pair<unsigned int, unsigned int>> Game::findPiece()
{
    SDL_Rect pivot; 
    
    try
    {
        pivot = findPivot();
    }
    catch(std::exception)
    {
        return std::vector<std::pair<unsigned int, unsigned int>>();
    }

    std::vector<std::pair<unsigned int, unsigned int>> piece;

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



