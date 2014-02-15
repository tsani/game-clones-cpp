#include "GameOverState.h"

#include "Application.h"

GameOverState::GameOverState(Application *a_parent, unsigned int a_score, unsigned int a_speed)
    : State(a_parent), m_scoreSurface(nullptr), m_levelSurface(nullptr), m_gameOverSurface(nullptr),
      m_score(a_score), m_level(a_speed), m_font(nullptr), m_textColorFg(SDL_Color { 255, 255, 255 }),
      m_textColorBg (SDL_Color { 0, 0, 0 })
{

}

void GameOverState::load()
{
    std::stringstream sb;

    m_font = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 20));

    m_gameOverSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_font.get(), "YOU ARE A TETRIS MASTER",
                                                                 m_textColorFg, m_textColorBg));

    sb << "Score: " << m_score;
    m_scoreSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_font.get(), sb.str().c_str(), 
                                                              m_textColorFg, m_textColorBg));

    sb.clear();
    sb.str("");
    sb << "Level: " << m_level;
    m_levelSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_font.get(), sb.str().c_str(), 
                                                              m_textColorFg, m_textColorBg));

    State::load();
}

void GameOverState::handleEvent(SDL_Event const& event)
{
    if ( event.type == SDL_KEYUP )
        setState(AppState::finished);
}

void GameOverState::draw(Surface_ptr a_parent)
{
    static SDL_Rect gameOverTextPosition { a_parent->w / 2 - m_gameOverSurface->w / 2,
                                           a_parent->h / 2 - m_gameOverSurface->h / 2, 0, 0 };
    static SDL_Rect scoreTextPosition { gameOverTextPosition.x, gameOverTextPosition.y + 25, 0, 0 };
    static SDL_Rect levelTextPosition { gameOverTextPosition.x, scoreTextPosition.y + 25, 0, 0 };

    if ( SDL_BlitSurface (m_gameOverSurface.get(), NULL, a_parent.get(), &gameOverTextPosition)
            != 0 )
        std::cerr << "Failed to blit game over text surface." << std::endl;

    if ( SDL_BlitSurface (m_scoreSurface.get(), NULL, a_parent.get(), &scoreTextPosition)
            != 0 )
        std::cerr << "Failed to blit score surface." << std::endl;

    if ( SDL_BlitSurface (m_levelSurface.get(), NULL, a_parent.get(), &levelTextPosition)
            != 0 )
        std::cerr << "Failed to blit level surface." << std::endl;

    State::draw(a_parent);
}
