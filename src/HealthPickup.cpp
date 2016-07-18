#include "HealthPickup.h"

HealthPickup::HealthPickup(SDL_Texture* texture, const SDL_Rect& rect, const int health)
{
    this->texture = texture;
    this->rect = rect;

    this->rect.x += this->rect.w / 2;
    this->rect.y += this->rect.h / 2;
    SDL_QueryTexture(this->texture, nullptr, nullptr, &this->rect.w, &this->rect.h);
    this->rect.x -= this->rect.w / 2;
    this->rect.y -= this->rect.h / 2;

    this->health = health;
}

void HealthPickup::draw()
{
    SDL_Rect draw_rect = Application::convertToCameraView(rect);
    SDL_RenderCopy(Application::getRenderer(), texture, nullptr, &draw_rect);
}

SDL_Rect& HealthPickup::getRect()
{
    return rect;
}

int HealthPickup::getHealth()
{
    return health;
}
