#ifndef UTIL_SDL_H
#define UTIL_SDL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
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

struct TTF_FontDeleter
{
    void operator() (TTF_Font *ptr) const
    {
        TTF_CloseFont(ptr);
    }
};

typedef std::shared_ptr<SDL_Surface> Surface_ptr;
typedef std::shared_ptr<SDL_Surface> Screen_ptr;
typedef std::shared_ptr<SDL_Rect>    Rect_ptr;
typedef std::shared_ptr<TTF_Font>    Font_ptr;

/** This utility function produces a shared_ptr to an SDL_Surface with the correct deleter, namely
 * SDL_FreeSurface attached. This will result in the correct destruction of the surface when all
 * the shared pointers go out of scope or are destroyed.
 */
Surface_ptr makeSafeSurfacePtr(SDL_Surface *surface);

SDL_Surface * loadOptimizedImage(std::string const& path);

Font_ptr makeSafeFontPtr(TTF_Font *font);

Rect_ptr makeSafeRectPtr(short x, short y, unsigned short w, unsigned short h);

SDL_Rect makeRect(short x, short y, unsigned short w, unsigned short h);
#endif
