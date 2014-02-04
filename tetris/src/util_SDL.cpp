#include "util_SDL.h"

Surface_ptr makeSafeSurfacePtr(SDL_Surface *surface)
{
    return Surface_ptr { surface, SDL_SurfaceDeleter() };
}

