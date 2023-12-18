#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Vec2.h"

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer, Vec2i* size)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_QueryTexture(texture, nullptr, nullptr, &(size->x), &(size->y));
    return texture;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

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