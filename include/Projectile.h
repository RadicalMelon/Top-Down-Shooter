#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Application.h"
#include "Tools.h"

/**
* This class represents every projectile in the game.
*/
class Projectile
{
public:
    /**
    * Create a projectile with a texture, a start position, a set damage, a speed and a spread.
    */
    Projectile(SDL_Texture* texture, const int x, const int y, const int direction, const int damage, const int speed, const int spread);

    /**
    * This method updates the pojectile.
    */
    void update();

    /**
    * This method draws the projectile.
    */
    void draw();

    /**
    * This method gets the damage the projectile's damage.
    */
    int getDamage();

    /**
    * This method gets the rect for the projectile.
    */
    const SDL_Rect& getRect();

private:
    SDL_Texture* texture;
    SDL_Rect rect;
    Tools::FloatVector normal;
    int damage;
    int speed;
    int angle;
};

#endif // PROJECTILE_H
