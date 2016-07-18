#ifndef HEALTHPICKUP_H
#define HEALTHPICKUP_H

#include "Application.h"

class HealthPickup
{
public:
    HealthPickup(SDL_Texture* texture, const SDL_Rect& rect, const int health);
    void draw();
    SDL_Rect& getRect();
    int getHealth();

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    int health;
};

#endif // HEALTHPICKUP_H
