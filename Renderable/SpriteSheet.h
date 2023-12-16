#pragma once

#include "Renderable.h"

struct SpriteSheet :Renderable
{
    Vec2i position;
    Vec2i size;
    Vec2i spriteSize;
    SDL_Texture* texture;
    int currentIndex = 0;
    int maxIndex = 0;
    SDL_Rect parentRect;

    void _alignCenter()
    {
        if (parentRect.w != 0)
            this->position.x += (parentRect.w - spriteSize.x) / 2;

        if (parentRect.h != 0)
            this->position.y += (parentRect.h - spriteSize.y) / 2;
    }

    SDL_Rect spriteRect()
    {
        return { currentIndex * spriteSize.x, 0, spriteSize.x, spriteSize.y };
    }



    void load(const char* path, Vec2i position, Vec2i spriteSize, SDL_Rect parentRect, SDL_Renderer* renderer)
    {
        this->position = position;
        this->parentRect = parentRect;
        this->spriteSize = spriteSize;
        this->renderer = renderer;
        texture = loadTexture(path, renderer, &size);
        this->maxIndex = size.x / spriteSize.x - 1;
        _alignCenter();
    }

    void render()
    {
        SDL_Rect srcr = spriteRect();
        SDL_Rect dstr = { position.x, position.y, spriteSize.x, spriteSize.y };
        SDL_RenderCopy(renderer, texture, &srcr, &dstr);
    }

};