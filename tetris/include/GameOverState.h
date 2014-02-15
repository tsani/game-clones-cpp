#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include <iostream>
#include <sstream>

#include "util_SDL.h"
#include "State.h"

class Application;

class GameOverState
    : public State
{
    public:
        GameOverState (Application *a_parent, unsigned int a_score, unsigned int a_speed);

        virtual void load() override;
        // We don't need to override `cleanup`.
        // We don't need to override `update`.
        virtual void handleEvent(SDL_Event const& event) override;
        virtual void draw(Surface_ptr a_parent) override;

    private:
        Surface_ptr m_scoreSurface, m_levelSurface, m_gameOverSurface;
        unsigned int m_score, m_level;
        Font_ptr m_font;
        SDL_Color m_textColorFg, m_textColorBg;
};

#endif
