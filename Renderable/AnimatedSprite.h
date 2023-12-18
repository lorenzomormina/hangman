#pragma once

#include "Renderable.h"
#include <vector>

struct AnimatedSprite : Renderable
{
    Vec2i gridSize;
    Vec2i scaledSize;
    std::vector<Vec2i> frames;
    int duration;
    int currentFrame = 0;

    void render()
    {
        SDL_Rect srcRect;
        srcRect.x = frames[currentFrame].x * gridSize.x;
        srcRect.y = frames[currentFrame].y * gridSize.y;
        srcRect.w = gridSize.x;
        srcRect.h = gridSize.y;

        SDL_Rect dstRect;
        dstRect.x = position.x;
        dstRect.y = position.y;
        dstRect.w = scaledSize.x;
        dstRect.h = scaledSize.y;

        SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
    }

    void load(const char* path, Vec2i gridSize, SDL_Renderer* renderer)
    {
        texture = loadTexture(path, renderer);
        SDL_QueryTexture(texture, nullptr, nullptr, &this->size.x, &this->size.y);
        this->gridSize = gridSize;
    }
};