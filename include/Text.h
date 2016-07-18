#ifndef TEXT_H
#define TEXT_H

#include "Application.h"

class Text
{
public:
    Text(TTF_Font* font, const std::string& text, const int x, const int y, const bool centered, const int width = 0);
    void setText(const std::string& text);
    void draw();
    ~Text();

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    TTF_Font* font;
    bool centered;
    int width;
};

#endif // TEXT_H
