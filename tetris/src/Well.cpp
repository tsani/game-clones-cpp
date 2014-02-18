#include "Well.h"

Well::Well(unsigned int a_width, unsigned int a_height)
    : m_rdistribution(0, 6)
{
    m_well = std::vector<std::vector<bool>>(a_width);

    for ( WellMatrix::iterator i = m_well.begin(); i != m_well.end(); i++ )
    {
        (*i) = std::vector<bool>(a_height);
        for ( std::vector<bool>::iterator j = i->begin(); j != i->end(); j++ )
        {
            *j = false;
        }
    }

    m_wellWidth = a_width;
    m_wellHeight = a_height;

    m_nextPieceID = m_rdistribution(m_rengine);

    m_rotationID = 0;
}

Well::Well(WellMatrix a_initialMatrix)
{
    m_well = a_initialMatrix;

    unsigned int height = m_well[0].size();

    if ( std::any_of(m_well.begin(), m_well.end(), [height] (std::vector<bool> const& b) { return b.size() != height;  } ) )
        throw new std::exception (); // The heights are not uniform in the initial well.
}

// // // MUTATORS // // //

bool Well::updatePiece(Piece & p) const
{
    if ( std::any_of(p.begin(), p.end(), 
                [this] (Block b) 
                {
                    return b.location.second + 1 >= m_wellHeight || 
                        m_well[b.location.first][b.location.second + 1];
                }) )
    {
        return true;
    }
    else
    {
        for ( auto &q : p)
            q.location.second++;
        return false;
    }
}

bool Well::updatePiece()
{
    if ( updatePiece(m_piece) )
    {
        collidePiece();
        return true;
    }
        return false;
}

void Well::collidePiece(Piece const& p)
{
    for ( auto &q : p )
        setBlock(q.location, true);
}

void Well::collidePiece()
{
    collidePiece(m_piece);
}

bool Well::movePiece(int d)
{
    Piece p = movePiece(m_piece, d);

    if ( p != m_piece )
    {
        m_piece = p;
        return true;
    }
        return false;
}

Well::Piece Well::movePiece(Piece const& piece, int d)
{
    Piece newPiece = piece;
    unsigned int nx = 0;

    for ( auto &block : newPiece )
    {
        nx = block.location.first + d;
        if ( nx < m_wellWidth )
            block.location.first = nx;
        else
            return piece;
    }

    if ( wouldOverlap(newPiece) )
        return piece;
    else
        return newPiece;
}

void Well::fall()
{
    while ( ! updatePiece() );
}

void Well::fall(Piece & p) const
{
    while ( ! updatePiece(p) );
}

bool Well::newPiece()
{
    Piece newPiece = spawnPiece({ m_wellWidth / 2, 0 }, m_nextPieceID, 0);
    if ( newPiece.size() == 0 )
        return false;
    else
    {
        m_piece = newPiece;
        m_pieceID = m_nextPieceID;
        m_rotationID = 0;
        m_nextPieceID = m_rdistribution(m_rengine);
        return true;
    }
}

Well::Piece Well::spawnPiece(Point p, unsigned int pieceID, int rotationID) const
{
    Piece newPiece;

    for ( int i = 0; i < 5; i++ )
    {
        for ( int j = 0; j < 5; j++ )
        {
            if ( PIECES[pieceID][rotationID][j][i] == 0 )
                continue;

            unsigned int px = p.first - 2 + i, py = p.second - 2 + j;

            if ( px < m_wellWidth && py < m_wellHeight && ! m_well[px][py] )
            {
                Block b { {px, py}, PIECES[pieceID][rotationID][j][i] == 1 ? BlockState::falling : BlockState::pivot };
                newPiece.push_back(b);
            }
            else
            {
                return Piece();
            }
        }
    }

    return newPiece;
}

Well::Piece Well::rotatePiece(Piece const& piece, Direction d)
{
    int newRotationID = (m_rotationID + (int)d) % 4;

    if ( newRotationID < 0 )
        newRotationID = 3;

    Piece p = spawnPiece(std::find_if(piece.begin(), piece.end(),
                [] (Block const& b) {
                    return b.type == BlockState::pivot;
                } ) -> location, m_pieceID, newRotationID);

    if ( p.size() > 0 ) // successfully spawned the piece.
        m_rotationID = newRotationID;

    return p; // p will be empty if the spawn failed.
}

bool Well::rotatePiece(Direction d)
{
    Piece p = rotatePiece(m_piece, d);

    if ( p.size() > 0 )
    {
        m_piece = p;
        return true;
    }
    else
        return false;
}

void Well::removeRows(std::vector<unsigned int> const& a_rows)
{
    if ( a_rows.size() == 0 )
        return;

    for ( auto y : a_rows )
    {
        for ( unsigned int x = 0; x < m_wellWidth; x++ )
        {
            m_well[x][y] = false; // clear the row
        }
        for ( unsigned int y_ = y; y_ > 1; y_-- )
        {
            for ( unsigned int x = 0; x < m_wellWidth; x++ )
                m_well[x][y_] = m_well[x][y_ - 1]; // shift down rows above the removed row
        }
        for ( unsigned int x = 0; x < m_wellWidth; x++ )
        {
            m_well[x][0] = false; // clear the top row.
        }
    }
}

// // // OBSERVERS // // //

std::vector<unsigned int> Well::getFullRows() const
{
    std::vector<unsigned int> fullRows;

    for ( unsigned int y = 0; y < m_wellHeight; y++ )
    {
        bool full = true;
        for ( unsigned int x = 0; x < m_wellWidth; x++ )
        {
            if ( ! m_well[x][y] )
            {
                full = false;
                break;
            }
        }

        if ( full )
            fullRows.push_back(y);
    }

    return fullRows;
}

Well::WellMatrix const& Well::getWell() const
{
    return m_well;
}

Well::Piece const& Well::getPiece() const
{
    return m_piece;
}

Well::Piece Well::getFallenPiece() 
{
    static Piece result;

    if ( m_fallenPieceMemoID == m_pieceID && [] (Piece const& fallenPieceMemo, Piece const& piece) 
            {
                if ( fallenPieceMemo.size() != piece.size() )
                    return false;

                for ( unsigned int i = 0; i < fallenPieceMemo.size(); i++ )
                {
                    if ( fallenPieceMemo[i].location.first != piece[i].location.first )
                        return false;
                }
                return true;
            } (m_fallenPieceMemo, m_piece)
       )
        return result;
    else
    {
        Piece p(m_piece); // copy the current piece into a temporary;
        fall(p);

        m_fallenPieceMemo = m_piece;
        m_fallenPieceMemoID = m_pieceID;
        result = p;
        return p;
    }
}

bool Well::wouldOverlap(Piece const& p) const
{
    return std::any_of(p.begin(), p.end(), 
            [this] (Block b)
            {
                return getBlock(b.location);
            }
        );
}

unsigned int Well::getWellWidth() const
{
    return m_wellWidth;
}

unsigned int Well::getWellHeight() const
{
    return m_wellHeight;
}

unsigned int Well::getNextPieceID() const 
{
    return m_nextPieceID;
}

unsigned int Well::getPieceID() const
{
    return m_pieceID;
}

bool Well::getBlock(Point const& p) const
{
    return m_well[p.first][p.second];
}

void Well::setBlock(Point const& p, bool b) 
{
    m_well[p.first][p.second] = b;
}
