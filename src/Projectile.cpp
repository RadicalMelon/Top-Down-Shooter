#include "Projectile.h"

/**
* Create a projectile with a texture, a start position, a set damage, a speed and a spread.
*/
Projectile::Projectile(SDL_Texture* texture, const int x, const int y, const int direction, const int damage, const int speed, const int spread)
{
    this->texture = texture;
    this->damage = damage;
    this->speed = speed;

    SDL_QueryTexture(this->texture, nullptr, nullptr, &rect.w, &rect.h);
    rect.x = x - (rect.w / 2);
    rect.y = y - (rect.h / 2);

    angle = direction;

    // Apply spread to projectile.
    angle += Tools::getRandomInt(-spread, spread);

    // Convert the angle to radians.
    normal.x = std::cos(angle * 0.0174533);
    normal.y = std::sin(angle * 0.0174533);

    // Normalize the vector.
    normal = Tools::normalizeVector(normal);

    normal.x *= this->speed;
    normal.y *= this->speed;
}

/**
* This method updates the pojectile.
*/
void Projectile::update()
{
    // Move the projectile.
    rect.x += std::round(normal.x * Application::getDeltaTime());
    rect.y += std::round(normal.y * Application::getDeltaTime());
}

/**
* This method draws the projectile.
*/
void Projectile::draw()
{
    // Apply the camera to the projectile.
    SDL_Rect draw_rect = Application::convertToCameraView(rect);
    SDL_RenderCopyEx(Application::getRenderer(), texture, nullptr, &draw_rect, angle, nullptr, SDL_FLIP_NONE);
}

/**
* This method gets the damage the projectile's damage.
*/
int Projectile::getDamage()
{
    return damage;
}

/**
* This method gets the rect for the projectile.
*/
const SDL_Rect& Projectile::getRect()
{
    return rect;
}
