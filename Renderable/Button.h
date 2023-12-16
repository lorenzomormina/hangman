#pragma once

#include "Renderable.h"
#include "Text.h"

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

    void render()
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
};