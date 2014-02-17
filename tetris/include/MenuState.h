#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <vector>
#include <memory>

#include "State.h"
#include "ForState.h"
#include "util_SDL.h"

class Application;

class MenuState final
    : public State
{
    public:
        MenuState(const Application *a_parent);

        virtual void load() override;
        //virtual void cleanup() override;
        //virtual void update() override;
        virtual void draw(Surface_ptr a_parent) override;
        virtual void handleEvent(SDL_Event const& event) override;

        static const int maxSpeed = 20;
        static const short buttonSpacing = 5;

    private:
        const Application* getOwner()
        {
            return (const Application*)(m_parent);
        }

        Font_ptr m_titleFont,
                 m_menuFont;

        SDL_Rect m_titlePosition;
        Surface_ptr m_titleText;

        SDL_Rect m_initialSpeedPosition;
        unsigned short m_boxWidth, m_boxHeight;
        Surface_ptr m_selectedSpeedSurface, m_unselectedSpeedSurface;
        std::vector<Surface_ptr> m_initialSpeedTexts;

        int m_selectedSpeed;
};

#endif

