#ifndef GAME_H
#define GAME_H

#include "util_SDL.h"

#include <map>
#include <memory>
#include <random>
#include <functional>

extern const char PIECES[7][4][5][5]; // Defined in TetrisData.cpp

class Application;

enum class BlockState
{
    pivot   = 3,
    falling = 2,  // refers to pieces that are affected by gravity
    fallen  = 1,  // refers to pieces that have collided with either the bottom or other fallen pieces
    free    = 0   // refers to an empty block
};

class Game
{
    public:
        Game(Application* a_owner);

        bool load();
        void cleanup();
        void update();
        void handleEvent(SDL_Event const& event);
        void draw(Surface_ptr a_parent);


        static const unsigned short wellWidth = 10;
        static const unsigned short wellHeight = 20;
        static const unsigned short blockSide = 32;

    private:
        /** Effects gravity to the falling blocks, shifting them down one, or changing them to fallen
         * blocks if they collide below.
         * Returns true if a new piece was spawned <=> the current piece collided with the ground.
         */
        bool updateBlocks();

        /** Changes this block, and all adjacent falling blocks, into fallen blocks, recursively.
         */
        void collideBlocks(int x, int y);

        /** Creates a falling piece of the specified type with the specified rotation with its pivot block at the specified coordinates. 
         * This method also sets m_pieceID and m_rotationLevel to the provided values.
         * Returns false if: 
         *   * a portion of the piece would be outside the bounds of the well.
         *   * a portion of the piece would be colliding with a fallen block.
         * Otherwise, returns true.
         * Throws an exception if:
         *   * the given coordinates are outside the bounds of the well.
         *   * the pieceID or rotationID are out of bounds. (Outside [0, 6] or [0, 3], respectively.)
         *
         * The special value of -1 (which is the default) can be supplied as a pieceID, in which case, a random piece is created.
         * Called only with its default values, this method will spawn a random piece at the top of the well.
         */
        bool spawnPiece(int x = 5, int y = 0, int pieceID = -1, int rotationID = 0);

        SDL_Rect findPivot();

        std::vector<std::pair<int, int>> findPiece();

        /** Check the well to see if a line has been filled horizontally.
         * Called by updateBlocks only in the event of a collision.
         */
        void checkTetris();

        /** Rotates the piece clockwise, about the position of its pivot block.
         * Only the sign of the parameter is relevant and is as in mathematics: positive is counterclockwise and negative is clockwise.
         * The return value represents the success of the operation.
         */
        bool rotate(int direction);

        /** Changes all the falling and pivot blocks into free blocks, effectively removing the falling piece.
         */
        void removeFallingPiece();

        /* Moves the piece horizontally, relative to its current position.
         * Return value represents the success of the movement.
         * Only the sign of direction is relevant. Positive is right, negative is left.
         */
        bool movePiece(unsigned int dx, int direction, std::vector<std::pair<int, int>> & piece);

        bool movePiece(unsigned int dx, int direction, std::vector<std::pair<int, int>> && piece)
        {
            return movePiece(dx, direction, piece);
        }

        bool movePiece(unsigned int dx, int direction)
        {
            return movePiece(dx, direction, findPiece());
        }

        /** Pushes the falling piece into the ground, causing it to become fallen.
         */
        void fall();

        unsigned int m_score; // the player's score (unused)
        unsigned int m_speed;    // the inverse speed of the falling blocks
        int m_rotationLevel; // how rotated the current piece is. Is a valid index into a sub-dimension of PIECES
        int m_pieceID;       // the index of the current piece

        BlockState m_well[wellWidth][wellHeight]; // the well data
        SDL_Rect m_wellPosition; // pos. of top-left corner of the well (such that it is centered onscreen)

        std::map<const BlockState, Surface_ptr> m_blockSurfaces; // the surfaces associated with the blocks depending on whether they are falling or fallen.

        std::default_random_engine m_rengine; // for the randomly generated block types
        std::uniform_int_distribution<int> m_rdistribution;

        Application* m_owner;    // used to get a reference to the screen.
        std::map<Uint8, unsigned int> m_keyTimes; // for fancy input handling (ignored)
        const Uint8 * m_keyboard; // for fancy input handling (ignored)
};

#endif
