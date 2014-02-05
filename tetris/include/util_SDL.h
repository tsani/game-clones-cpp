#ifndef UTIL_SDL_H
#define UTIL_SDL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <memory>

template<typename T>
struct EmptyDeleter
{
    void operator() (T *ptr) const
    {
    }
};

struct SDL_SurfaceDeleter
{
    void operator() (SDL_Surface *ptr) const
    {
        SDL_FreeSurface(ptr);
    }
};

typedef std::shared_ptr<SDL_Surface> Surface_ptr;
typedef std::shared_ptr<SDL_Surface> Screen_ptr;
typedef std::shared_ptr<SDL_Rect>    Rect_ptr;

/** This utility function produces a shared_ptr to an SDL_Surface with the correct deleter, namely
 * SDL_FreeSurface attached. This will result in the correct destruction of the surface when all
 * the shared pointers go out of scope or are destroyed.
 */
Surface_ptr makeSafeSurfacePtr(SDL_Surface *surface);

SDL_Surface * loadOptimized(std::string const& path);

Rect_ptr makeSafeRectPtr(short x, short y, unsigned short w, unsigned short h);

SDL_Rect makeRect(short x, short y, unsigned short w, unsigned short h);
#endif
