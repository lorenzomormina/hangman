#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "Vec2.h"
#include "Renderable/Renderable.h"
#include "Renderable/Text.h"

struct Button : Renderable
{
    Text text;
    SDL_Color color;
    SDL_Color textColor;

    void init(Vec2i size, Vec2i position, SDL_Color color, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer)
    {
        this->renderer = renderer;
        this->size = size;
        this->position = position;
        this->color = color;
        this->text.init(renderer, font, textColor, position, { position.x, position.y, size.x, size.y });
    }

    void init(const std::string& text, Vec2i size, Vec2i position, SDL_Color color, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer)
    {
        this->renderer = renderer;
        this->size = size;
        this->position = position;
        this->color = color;
        this->text.init(renderer, font, textColor, position, { position.x, position.y, size.x, size.y });
        this->text.setValue(text);
    }

    void render();
};

struct AnimatedSprite : Renderable
{
    Vec2i position;
    Vec2i size;
    Vec2i gridSize;
    Vec2i scaledSize;
    SDL_Texture* texture;
    std::vector<Vec2i> frames;
    int duration;
    int currentFrame = 0;
    void load(const char* path, Vec2i gridSize, SDL_Renderer* renderer);
    void render();
};

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer, Vec2i* size)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_QueryTexture(texture, nullptr, nullptr, &(size->x), &(size->y));

    return texture;
}

// ---


void Button::render()
{
    SDL_Rect buttonRect = { position.x, position.y, size.x, size.y };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Rect textRect;
    TTF_SizeText(text.font, text.value.c_str(), &textRect.w, &textRect.h);
    textRect.x = position.x + (size.x - textRect.w) / 2;
    textRect.y = position.y + (size.y - textRect.h) / 2;

    SDL_RenderCopy(renderer, text.texture, nullptr, &textRect);

}

void AnimatedSprite::render()
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

void AnimatedSprite::load(const char* path, Vec2i gridSize, SDL_Renderer* renderer)
{
    texture = loadTexture(path, renderer);
    SDL_QueryTexture(texture, nullptr, nullptr, &this->size.x, &this->size.y);
    this->gridSize = gridSize;
}

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer)
{
    auto surface = IMG_Load(path);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}