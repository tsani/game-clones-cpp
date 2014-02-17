#include "MenuState.h"
#include "Application.h"

MenuState::MenuState(const Application *a_parent)
    : State(a_parent), m_selectedSpeed(0)
{

}

void MenuState::load()
{
    m_titleFont = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 48));
    m_menuFont  = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 24));

    m_titleText = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_titleFont.get(), 
                                                           "TETRIS", 
                                                           SDL_Color {255, 255, 255}, 
                                                           SDL_Color {0,   0,   0  }));

    std::stringstream ss;
    for ( unsigned int i = 0; i < maxSpeed; i++ )
    {
        ss.clear();
        ss.str("");
        ss << i + 1;
        m_initialSpeedTexts.push_back(makeSafeSurfacePtr(TTF_RenderText_Blended(m_menuFont.get(),
                                                                                ss.str().c_str(),
                                                                                SDL_Color {0, 0, 0})));
    }

    m_boxWidth = (*std::max_element(m_initialSpeedTexts.begin(), m_initialSpeedTexts.end(),
            [] (Surface_ptr a, Surface_ptr b) {
                return a->w < b->w;
            }))->w + 5;
    m_boxHeight = (*std::max_element(m_initialSpeedTexts.begin(), m_initialSpeedTexts.end(), 
            [] (Surface_ptr a, Surface_ptr b) {
                return a->h < b->h;
            }))->h;

    m_selectedSpeedSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, m_boxWidth, 
                m_boxHeight, Application::screenDepth, 0, 0, 0, 0));
    m_unselectedSpeedSurface = makeSafeSurfacePtr(SDL_CreateRGBSurface(SDL_HWSURFACE, m_boxWidth,
                m_boxHeight, Application::screenDepth, 0, 0, 0, 0));
    SDL_FillRect(m_selectedSpeedSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 255, 128, 128));
    SDL_FillRect(m_unselectedSpeedSurface.get(), nullptr, SDL_MapRGB(getOwner()->getScreen().lock()->format, 128, 255, 128));

    m_titlePosition = SDL_Rect { (short)(getOwner()->getScreen().lock()->w / 2 - m_titleText->w / 2),
                                 (short)(m_titleText->h / 2 + 100), 0, 0 };
    m_initialSpeedPosition = SDL_Rect { (short)(getOwner()->getScreen().lock()->w / 2 - (m_boxWidth * maxSpeed + buttonSpacing * (maxSpeed - 1)) / 2),
                                        (short)(m_titlePosition.y + m_titleText->h + 100), 0, 0 };

    State::load();
}

void MenuState::draw(Surface_ptr a_parent)
{
    SDL_BlitSurface(m_titleText.get(), nullptr, a_parent.get(), &m_titlePosition);

    SDL_Rect drawLocation;

    for ( int i = 0; i < maxSpeed; i++ )
    {
        drawLocation = m_initialSpeedPosition;
        drawLocation.x += i * m_boxWidth + (i * buttonSpacing);

        SDL_BlitSurface(i == m_selectedSpeed ? 
                m_selectedSpeedSurface.get() : m_unselectedSpeedSurface.get(),
                nullptr, a_parent.get(), &drawLocation);

        drawLocation.x += m_boxWidth / 2 - m_initialSpeedTexts[i]->w / 2;
        drawLocation.y += m_boxHeight / 2 - m_initialSpeedTexts[i]->h / 2;

        SDL_BlitSurface(m_initialSpeedTexts[i].get(), nullptr, a_parent.get(), &drawLocation);
    }
}

void MenuState::handleEvent(SDL_Event const& event)
{
    if ( event.type == SDL_KEYUP )
    {
        switch ( event.key.keysym.sym )
        {
            case SDLK_RETURN:
                setState(AppState::finished);
                m_next = std::make_shared<Game>(getOwner(), m_selectedSpeed + 1);
                break;
            case SDLK_LEFT:
                if ( --m_selectedSpeed < 0 )
                    m_selectedSpeed = maxSpeed - 1;
                break;
            case SDLK_RIGHT:
                if ( ++m_selectedSpeed >= maxSpeed )
                    m_selectedSpeed = 0;
                break;
            default:
                break;
        }
    }
}
    
