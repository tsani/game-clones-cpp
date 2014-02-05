#ifndef GAME_H
#define GAME_H

#include "util_SDL.h"

#include <map>
#include <memory>
#include <random>
#include <functional>

extern char PIECES[7][4][5][5]; // Defined in TetrisData.cpp

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
         */
        void updateBlocks();

        /** Changes this block, and all adjacent falling blocks, into fallen blocks, recursively.
         */
        void collideBlocks(unsigned int x, unsigned int y);

        /** Creates a new piece at the top of the screen.
         */
        void spawnPiece();

        unsigned int m_score;
        BlockState m_well[wellWidth][wellHeight];
        std::map<const BlockState, Surface_ptr> m_blockSurfaces;
        std::default_random_engine m_rengine;
        std::uniform_int_distribution<int> m_rdistribution;
        Application* m_owner;
};

#endif
