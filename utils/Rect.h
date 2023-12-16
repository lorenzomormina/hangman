#pragma once

#include <SDL2/SDL.h>
#include "Vec2.h"

struct Rect
{
    SDL_Rect value;

    Rect()
    {
        value = SDL_Rect{ 0, 0, 0, 0 };
    }

    Rect(const Rect& other)
    {
        value = other.value;
    }

    Rect(int x, int y, int w, int h)
    {
        value = SDL_Rect{ x, y, w, h };
    }

    Rect(Vec2i pos, Vec2i size)
    {
        value = SDL_Rect{ pos.x, pos.y, size.x, size.y };
    }

    SDL_Rect SDLRect()
    {
        return value;
    }

    int x() const
    {
        return value.x;
    }

    void x(int val)
    {
        value.x = val;
    }

    int y() const
    {
        return value.y;
    }

    void y(int val)
    {
        value.y = val;
    }

    int w() const
    {
        return value.w;
    }

    int h() const
    {
        return value.h;
    }

    Vec2i pos()
    {
        return Vec2i{ value.x, value.y };
    }

    void pos(Vec2i val)
    {
        value.x = val.x;
        value.y = val.y;
    }

    Vec2i size()
    {
        return Vec2i{ value.w, value.h };
    }

    Vec2i center()
    {
        return Vec2i{ value.x + value.w / 2, value.y + value.h / 2 };
    }

    bool containsPoint(Vec2i point)
    {
        return point.x >= value.x && point.x <= value.x + value.w && point.y >= value.y && point.y <= value.y + value.h;
    }

    bool containsPoint(int x, int y)
    {
        return x >= value.x && x <= value.x + value.w && y >= value.y && y <= value.y + value.h;
    }
};