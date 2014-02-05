#include "util_SDL.h"

Surface_ptr makeSafeSurfacePtr(SDL_Surface *surface)
{
    return Surface_ptr { surface, SDL_SurfaceDeleter() };
}

SDL_Surface *loadOptimized(std::string const& path)
{
    SDL_Surface *raw = nullptr, *optimized = nullptr;

    raw = IMG_Load(path.c_str());
    optimized = SDL_DisplayFormat(raw);
    SDL_FreeSurface(raw);

    return optimized;
}

SDL_Rect makeRect(short x, short y, unsigned short w, unsigned short h)
{
    return SDL_Rect { x, y, w, h };
}

Rect_ptr makeSafeRectPtr(short x, short y, unsigned short w, unsigned short h)
{
    return Rect_ptr { new SDL_Rect { x, y, w, h } };
}

