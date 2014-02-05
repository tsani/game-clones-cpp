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

