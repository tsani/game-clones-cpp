#ifndef GAME_H
#define GAME_H

#include <map>
#include <memory>
#include <random>
#include <functional>
#include <string>
#include <sstream>
#include <cmath>

#include "State.h"
#include "util_SDL.h"
#include "GameOverState.h"
#include "Well.h"
#include "ForState.h"
#include "MultiState.h"

extern const char PIECES[7][4][5][5]; // Defined in TetrisData.cpp

class Application;

class GameOverException
    : public std::exception
{
    
};

class Game final
    : public State
{
    public:
        Game(const Application* a_owner, unsigned int a_initialSpeed);

        virtual void load() override;
        virtual void cleanup() override;
        virtual void update() override;
        virtual void handleEvent(SDL_Event const& event) override;
        virtual void draw(Surface_ptr a_parent) override;

        unsigned int getScore()
        {
            return m_score;
        }

        unsigned int getLevel()
        {
            return m_speed;
        }

        static const unsigned short wellWidth    = 10;
        static const unsigned short wellHeight   = 20;
        static const unsigned short blockSide    = 32;
        static const unsigned short pieceStartX  = 5;
        static const unsigned short pieceStartY  = 0;
        static const unsigned int   baseRowScore = 100;
        static const unsigned int   speedStep    = 10;
        static const unsigned int   speedLimit   = 100;

        static const unsigned int   statusChangeEffectTime = 45;

    private:
        // Will call getFullRows, and if there are full rows, increase the score accordingly, increase the count of cleared rows, remove the fallen rows, and call handleSpeed.
        void handleRows();

        // Will effectively increase m_speed for every speedStep lines cleared.
        void handleSpeed();

        // Generate a new piece, and check for game over
        void handleNewPiece();

        void handleGenNewPiece();

        // Draws the preview box, where the next piece is shown.
        void drawPreviewBox(Surface_ptr a_parent);

        void renderStatus(Surface_ptr * dest, std::string const& text, unsigned int value, SDL_Color fg);
        void renderStatusWithEffect(Surface_ptr * dest, std::string const& text, unsigned int value, SDL_Color fg1, SDL_Color fg2, unsigned int delayTime);
        void renderStatusWithDefaultEffect(Surface_ptr * dest, std::string const& text, unsigned int value);

        const Application *getOwner()
        {
            return (const Application*)m_parent;
        }

        MultiState * getChild()
        {
            return (MultiState*)m_child.get();
        }

        Well m_well;

        bool m_fallFaster, m_falling, m_easyMode;

        unsigned int m_time; // how long has the current piece been in the screen

        unsigned int m_score; // the player's score 
        unsigned int m_clearedLines; // number of lines cleared by the player
        unsigned int m_speed;    // the inverse speed of the falling blocks

        SDL_Rect m_wellPosition; // pos. of top-left corner of the well (such that it is centered onscreen)
        SDL_Rect m_piecePreviewPosition; // pos. of top-left corner of the "next piece" boxr.

        Surface_ptr m_pieceSurface,
                    m_fallenSurface,
                    m_freeSurface,
                    m_fallenPreviewSurface;

        Font_ptr m_statusFont;   // the font used to draw the level number and the score.
        Surface_ptr m_scoreSurface, // the text is rendered into these surfaces only when it changes.
                    m_levelSurface, // this avoids unnecessary rerendering of the same text.
                    m_linesSurface;
        SDL_Rect m_statusLocation;  // where the game status is rendered;
        // Only one SDL_Rect is necessary, because the level is drawn just a bit lower than the score.

        SDL_Color m_statusColorFgNormal;    // the color the status text is drawn in.
        SDL_Color m_statusColorFgEffect;    // used when the value changes.
        SDL_Color m_statusColorBg;

        Surface_ptr m_clearedSurface;
        std::vector<SDL_Rect> m_clearingSurfaces;
};

#endif
