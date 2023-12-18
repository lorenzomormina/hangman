#pragma once

#include <SDL2/SDL.h>
#include "../Vec2.h"

struct Renderable
{
    Vec2i position;
    Vec2i size;
    SDL_Rect parentRect;
    SDL_Texture* texture;
    SDL_Renderer* renderer;

    SDL_Rect rect()
    {
        return { position.x, position.y, size.x, size.y };
    }

    virtual void render()
    {
        SDL_RenderCopy(renderer, texture, nullptr, &rect());
    }

    void _alignCenter()
    {
        if (parentRect.w != 0)
            this->position.x += (parentRect.w - size.x) / 2;

        if (parentRect.h != 0)
            this->position.y += (parentRect.h - size.y) / 2;
    }

    bool containsPoint(Vec2i point)
    {
        return point.x >= position.x && point.x <= position.x + size.x &&
            point.y >= position.y && point.y <= position.y + size.y;
    }

    bool containsPoint(int x, int y)
    {
        return x >= position.x && x <= position.x + size.x &&
            y >= position.y && y <= position.y + size.y;
    }
};