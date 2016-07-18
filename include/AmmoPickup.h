#ifndef AMMOPICKUP_H
#define AMMOPICKUP_H

#include "Application.h"
#include "Player.h"

/**
* This class represents all of the ammo pickups in the game.
* this can also be used for weapon pickups (and it is).
*/
class AmmoPickup
{
public:
    AmmoPickup(SDL_Texture* texture, const SDL_Rect& rect, const int ammo, const Weapon& weapon);
    void draw();
    const SDL_Rect& getRect();
    int getAmmo();
    const Weapon& getWeapon();

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    int ammo;
    Weapon weapon;
};

#endif // AMMOPICKUP_H
