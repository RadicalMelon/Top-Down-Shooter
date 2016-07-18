#include "Text.h"

Text::Text(TTF_Font* font, const std::string& text, const int x, const int y, const bool centered, const int width)
{
    this->font = font;
    this->centered = centered;
    this->width = width;

    setText(text);

    rect.x = x;
    rect.y = y;
    if (centered)
    {
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
    }
}

void Text::setText(const std::string& text)
{
    SDL_Surface* text_surface;
    if (width != 0)
    {
        text_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), {255, 255, 255, 255}, width);
    }
    else
    {
        text_surface = TTF_RenderText_Blended(font, text.c_str(), {255, 255, 255, 255});
    }

    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTextureFromSurface(Application::getRenderer(), text_surface);
    SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
    SDL_FreeSurface(text_surface);
}

void Text::draw()
{
    SDL_RenderCopy(Application::getRenderer(), texture, nullptr, &rect);
}

Text::~Text()
{
    SDL_DestroyTexture(texture);
}
