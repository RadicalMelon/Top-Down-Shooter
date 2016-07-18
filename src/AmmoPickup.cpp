#include "AmmoPickup.h"

AmmoPickup::AmmoPickup(SDL_Texture* texture, const SDL_Rect& rect, const int ammo, const Weapon& weapon)
{
    this->texture = texture;
    this->rect = rect;

    this->rect.x += this->rect.w / 2;
    this->rect.y += this->rect.h / 2;
    SDL_QueryTexture(this->texture, nullptr, nullptr, &this->rect.w, &this->rect.h);
    this->rect.x -= this->rect.w / 2;
    this->rect.y -= this->rect.h / 2;

    this->ammo = ammo;
    this->weapon = weapon;
}

void AmmoPickup::draw()
{
    SDL_Rect draw_rect = Application::convertToCameraView(rect);
    SDL_RenderCopy(Application::getRenderer(), texture, nullptr, &draw_rect);
}

const SDL_Rect& AmmoPickup::getRect()
{
    return rect;
}

int AmmoPickup::getAmmo()
{
    return ammo;
}

const Weapon& AmmoPickup::getWeapon()
{
    return weapon;
}
