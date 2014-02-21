#include "GameOverState.h"
#include "MenuState.h"
#include "Application.h"

GameOverState::GameOverState(const Application *a_parent, unsigned int a_score, unsigned int a_speed, unsigned int a_lines)
    : State(a_parent), m_scoreSurface(nullptr), m_levelSurface(nullptr), m_skillSurface(nullptr), m_linesSurface(nullptr), m_gameOverSurface(nullptr),
      m_score(a_score), m_level(a_speed), m_lines(a_lines), m_font(nullptr), m_textColorFg(SDL_Color { 255, 255, 255 }),
      m_textColorBg (SDL_Color { 0, 0, 0 })
{
    m_skill = m_lines == 0 ? 0 : (int)((double)m_score / (double)m_lines);
}

void GameOverState::load()
{
    m_font = makeSafeFontPtr(TTF_OpenFont("resources/statusfont.ttf", 20));

    m_gameOverSurface = makeSafeSurfacePtr(TTF_RenderText_Shaded(m_font.get(), "YOU ARE A TETRIS MASTER",
                                                                 m_textColorFg, m_textColorBg));

    auto makeStatus = [this] (std::string label, unsigned int value)
    {
        std::stringstream text;
        text << label << value;
        return makeSafeSurfacePtr(TTF_RenderText_Shaded(m_font.get(), text.str().c_str(),
                    m_textColorFg, m_textColorBg));
    };

    m_scoreSurface = makeStatus("Score: ", m_score);

    m_levelSurface = makeStatus("Level: ", m_level);

    m_linesSurface = makeStatus("Lines: ", m_lines);

    m_skillSurface = makeStatus("Skill: ", m_skill);

    State::load();
}

void GameOverState::handleEvent(SDL_Event const& event)
{
    if ( event.type == SDL_KEYUP )
    {
        setState(AppState::finished);
        m_next = std::make_shared<MenuState>(getOwner());
    }
}

void GameOverState::draw(Surface_ptr a_parent)
{
    static SDL_Rect gameOverTextPosition { (short)(a_parent->w / 2 - m_gameOverSurface->w / 2),
                                           (short)(a_parent->h / 3 - m_gameOverSurface->h / 2), 0, 0 };
    static SDL_Rect scoreTextPosition { gameOverTextPosition.x, (short)(gameOverTextPosition.y + m_gameOverSurface->h + 50), 0, 0 };
    static SDL_Rect levelTextPosition { gameOverTextPosition.x, (short)(scoreTextPosition.y + m_scoreSurface->h + 5), 0, 0 };
    static SDL_Rect linesTextPosition { gameOverTextPosition.x, (short)(levelTextPosition.y + m_levelSurface->h + 5), 0, 0 };
    static SDL_Rect skillTextPosition { gameOverTextPosition.x, (short)(linesTextPosition.y + m_linesSurface->h + 5), 0, 0 };

    if ( SDL_BlitSurface (m_gameOverSurface.get(), nullptr, a_parent.get(), &gameOverTextPosition)
            != 0 )
        std::cerr << "Failed to blit game over text surface." << std::endl;

    if ( SDL_BlitSurface (m_scoreSurface.get(), nullptr, a_parent.get(), &scoreTextPosition)
            != 0 )
        std::cerr << "Failed to blit score surface." << std::endl;

    if ( SDL_BlitSurface (m_levelSurface.get(), nullptr, a_parent.get(), &levelTextPosition)
            != 0 )
        std::cerr << "Failed to blit level surface." << std::endl;

    if ( SDL_BlitSurface (m_linesSurface.get(), nullptr, a_parent.get(), &linesTextPosition)
            != 0 )
        std::cerr << "Failed to blit lines surface." << std::endl;

    if ( SDL_BlitSurface (m_skillSurface.get(), nullptr, a_parent.get(), &skillTextPosition)
            != 0 )
        std::cerr << "Failed to blit skill surface." << std::endl;

    State::draw(a_parent);
}
