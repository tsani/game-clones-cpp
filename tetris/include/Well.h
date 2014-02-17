#ifndef WELL_H
#define WELL_H

#include <algorithm>
#include <random>

extern const char PIECES[7][4][5][5];

enum class BlockState : int
{
    pivot   = 3,
    falling = 2,  // refers to pieces that are affected by gravity
};

enum class Direction : int
{
    CW = -1,
    CCW = 1
};

typedef std::pair<unsigned int, unsigned int> Point;

struct Block
{
    Point location;
    BlockState type;

    Block(Point p, BlockState t)
        : location(p), type(t)
    {
    }

    bool operator==(Block const& b) const
    {
        return this->location == b.location && this->type == b.type;
    }
};

class Well final
{
    public:
        typedef std::vector<std::vector<bool>> WellMatrix;
        typedef std::vector<Block> Piece;

        // simply returns true iff b is pivot or falling.
        static bool isPieceComponent(BlockState b)
        {
            return b == BlockState::falling || b == BlockState::pivot;
        }

        // // // CONSTRUCTORS // // // 
        
        Well(unsigned int a_width, unsigned int a_height);
        Well(WellMatrix a_initialWell);

        // // // MUTATORS // // //  

        // Effects gravity onto the falling piece, checking and handling collision.
        // The return value represents whether the piece has been set into fallen blocks.
        bool updatePiece();

        // Generates the piece identified by m_nextPieceID and generates a new value for m_nextPieceID at the default spawn point at the top.
        bool newPiece();

        // Moves the falling piece `distance` blocks over. Negative is left and position is right.
        // The function is recursive and will move the piece as far as is possible in the given direction.
        // The return value represents whether the movement successfully completed.
        bool movePiece(int distance);

        // Rotates the falling piece. Returns true iff the rotation was successful.
        bool rotatePiece(Direction d);

        // Causes the falling piece to fall to the bottom, colliding it.
        // Simply calls `updateBlocks` in a loop.
        void fall();

        // Removes rows from the well, shifting down rows above. 
        // WARNING: the list of rows to remove must be sorted !
        void removeRows(std::vector<unsigned int> const& a_rows);

        // // // OBSERVERS // // // 

        // A list of rows that are full.
        std::vector<unsigned int> getFullRows() const;

        // A representation of the current well.
        WellMatrix const& getWell() const;

        Piece const& getPiece() const;

        // A Piece representing the current piece as if `fall` had hypothetically been called.
        Piece getFallenPiece() const;

        unsigned int getWellWidth() const;
        unsigned int getWellHeight() const;
        unsigned int getNextPieceID() const;
        unsigned int getPieceID() const;

    private:
        // Iterates over the blocks in the piece, and changes the corresponding locations in the well
        // to being occupied.
        void collidePiece(Piece const& p);
        void collidePiece();

        // Whether this piece would overlap with any of the fallen blocks in the well.
        // (Used in rotations.)
        bool wouldOverlap(Piece const& p) const;

        bool updatePiece(Piece & p) const;

        // Sees if spawning a piece at the desired point with desired rotation is possible, and if so
        // returns the resulting piece. (m_piece is not affected)
        // Piece is empty if the spawn would be impossible.
        Piece spawnPiece(Point p, unsigned int pieceID, int rotationID) const;

        void fall(Piece & p) const;

        // If the return value is the same as the provided piece, the movement failed.
        Piece movePiece(Piece const& piece, int distance);

        Piece rotatePiece(Piece const& piece, Direction d);

        bool getBlock(Point const& p) const;
        void setBlock(Point const& p, bool b);

        WellMatrix m_well;
        Piece m_piece;
        
        unsigned int m_wellWidth, m_wellHeight;
        unsigned int m_pieceID, m_nextPieceID;
        int m_rotationID;

        std::default_random_engine m_rengine;
        std::uniform_int_distribution<int> m_rdistribution;
};

#endif
