#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "Vec2.h"

struct Renderable
{
    SDL_Renderer* renderer;
    virtual void render() = 0;
};

struct Text : Renderable
{
    TTF_Font* font;
    SDL_Color color;
    std::string value;
    SDL_Texture* texture = nullptr;
    Vec2i position;
    SDL_Rect parentRect;

    void init(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Vec2i position, SDL_Rect parentRect);

    void setValue(const std::string& str);
    void assignValue(int count, char c);
    void appendValue(const std::string& str);
    void setAt(int pos, char c);

    Vec2i getTextureSize();
    void updateTexture();

    void align();
    void render();
};

struct Button : Renderable
{
    Vec2i position;
    Vec2i size;
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

struct SpriteSheet : Renderable
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

// ---

void Text::init(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Vec2i position, SDL_Rect parentRect)
{
    this->renderer = renderer;
    this->font = font;
    this->color = color;
    this->position = position;
    this->parentRect = parentRect;
}

void Text::setValue(const std::string& str)
{
    value = str;
    updateTexture();
}

void Text::assignValue(int count, char c)
{
    value.assign(count, c);
    updateTexture();
}

void Text::appendValue(const std::string& str)
{
    value.append(str);
    updateTexture();
}

void Text::setAt(int pos, char c)
{
    value[pos] = c;
    updateTexture();
}

Vec2i Text::getTextureSize()
{
    if (texture != nullptr) {
        Vec2i size;
        SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);
        return size;
    }
    return Vec2i{ 0, 0 };
}

void Text::align()
{
    if (texture != nullptr)
    {
        auto size = getTextureSize();

        if (parentRect.w != 0)
            this->position.x = (parentRect.w - size.x) / 2;

        if (parentRect.h != 0)
            this->position.y = (parentRect.h - size.y) / 2;
    }
}

void Text::render()
{
    if (texture != nullptr) {
        auto size = getTextureSize();
        SDL_Rect r = { position.x, position.y, size.x, size.y };
        SDL_RenderCopy(renderer, texture, nullptr, &r);
    }
}

void Text::updateTexture()
{
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (!value.empty()) {
        auto surface = TTF_RenderText_Blended(font, value.c_str(), color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        align();
    }
}

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

void SpriteSheet::render()
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

void SpriteSheet::load(const char* path, Vec2i gridSize, SDL_Renderer* renderer)
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