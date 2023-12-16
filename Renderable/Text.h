#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>
#include "Renderable.h"

struct Text : Renderable
{
    TTF_Font* font;
    SDL_Color color;
    std::string value;

    void init(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Vec2i position, SDL_Rect parentRect)
    {
        this->renderer = renderer;
        this->font = font;
        this->color = color;
        this->position = position;
        this->parentRect = parentRect;
    }

    void init(SDL_Renderer* renderer, const std::string& text, TTF_Font* font, SDL_Color color, Vec2i position)
    {
        this->renderer = renderer;
        this->font = font;
        this->color = color;
        this->position = position;
        parentRect = { 0, 0, 0, 0 };
        setValue(text);
        updateTexture();
    }

    void setValue(const std::string& str)
    {
        value = str;
        updateTexture();
    }

    void assignValue(int count, char c)
    {
        value.assign(count, c);
        updateTexture();
    }

    void appendValue(const std::string& str)
    {
        value.append(str);
        updateTexture();
    }

    void setAt(int pos, char c)
    {
        value[pos] = c;
        updateTexture();
    }

    Vec2i getTextureSize()
    {
        if (texture != nullptr) {
            Vec2i size;
            SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);
            return size;
        }
        return Vec2i{ 0, 0 };
    }

    void updateTexture()
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

    void align()
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

    void render()
    {
        if (texture != nullptr) {
            auto size = getTextureSize();
            SDL_Rect r = { position.x, position.y, size.x, size.y };
            SDL_RenderCopy(renderer, texture, nullptr, &r);
        }
    }    
};