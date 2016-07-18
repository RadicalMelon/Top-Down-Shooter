#ifndef PLAYER_H
#define PLAYER_H

#include "Application.h"
#include "Tools.h"
#include "Text.h"
#include "Weapon.h"
#include "Enemy.h"

#include <algorithm>

/**
* This class represents the player, it handles player movement, shooting
* and everything else the player does.
*/
class Player
{
public:
    Player();

    /**
    * This method respawns the player by resetting some attributes.
    */
    void respawn();

    /**
    * This method handles the key presses that are relevant to the player.
    * This mostly includes movement.
    */
    void handleKeyPresses();

    /**
    * This method handles the key releases that are relevant to the player.
    * This mostly includes movement.
    */
    void handleKeyReleases();

    /**
    * This method updates the player. It moves the player and also stops the player
    * from moving solid objects.
    */
    void update(const std::vector<SDL_Rect>& wall_rects);

    /**
    * This method draws the player to the screen.
    */
    void draw();

    /**
    * This method sets the player's rect. This is mostly used to set the player's
    * starting position in each level.
    */
    void setRect(SDL_Rect& rect);

    /**
    * This method gets the player's rect.
    */
    SDL_Rect& getRect();

    /**
    * This method changes the player's current weapon.
    */
    void changeWeapon();

    /**
    * This method gets the player's current weapon.
    */
    const Weapon& getWeapon();

    /**
    * This method returns if you can shoot your weapon right now.
    */
    bool canShoot();

    /**
    * This method tells the player to start shooting.
    */
    void startShooting();

    /**
    * This method tells the player to stop shooting.
    */
    void stopShooting();

    /**
    * This method returns the player's centre.
    */
    SDL_Point getCentre();

    /**
    * This method adds ammo for a certain weapon. It returns whether or not
    * ammo has been added.
    */
    bool addAmmo(const Weapon& weapon, const int ammo);

    /**
    * This method returns whether or not the player has a weapon in their inventory.
    */
    bool hasWeapon(const Weapon& weapon);

    /**
    * This method adds a weapon to the player's inventory.
    */
    void addWeapon(const Weapon& weapon);

    /**
    * This method adds to the player's health.
    */
    bool addHealth(const int health);

    /**
    * This method damages the player.
    */
    void damage(const int damage);

    /**
    * This method gets the player's angle.
    */
    int getAngle();

    /**
    * This method checks if the player is dead.
    */
    bool isDead();

private:
    /**
    * This method updates the ammo counter on the HUD.
    */
    void updateAmmoCounter();

    /**
    * This method updates the health count on the HUD.
    */
    void updateHealthCount();

private:
    static const int SPEED = 450;
    static const int MAX_HEALTH = 100;

    // The time (in seconds) between each shot of each weapon.
    float shoot_delay = 0.0;
    const std::map<Weapon, float> WEAPON_DELAYS = {
        {Weapon::Handgun,         0.3},
        {Weapon::Shotgun,         0.7},
        {Weapon::AssaultRifle,    0.1},
        {Weapon::Minigun,        0.05},
        {Weapon::PlasmaRifle,     0.1}
    };

    // The maximum amount of ammo every weapon can have.
    const std::map<Weapon, int> MAX_AMMO = {
        {Weapon::Handgun,         100},
        {Weapon::Shotgun,          50},
        {Weapon::AssaultRifle,    250},
        {Weapon::Minigun,         500},
        {Weapon::PlasmaRifle,     150}
    };

    // The amount of ammo every weapon starts with, the handgun starts with 25 since
    // the handgun is the weapon you start with.
    std::map<Weapon, int> ammo = {
        {Weapon::Handgun,         25},
        {Weapon::Shotgun,          0},
        {Weapon::AssaultRifle,     0},
        {Weapon::Minigun,          0},
        {Weapon::PlasmaRifle,      0}
    };

    int health = MAX_HEALTH;

    SDL_Texture* texture;
    SDL_Rect rect;
    Tools::FloatVector movement;
    int angle;

    // More stuff to do with weapons.
    bool shooting = false;
    Weapon current_weapon = Weapon::Handgun;
    int weapon_index = 0;
    std::vector<Weapon> weapons = {Weapon::Handgun};

    // HUD stuff.
    Text ammo_counter;
    Text health_count;
    SDL_Texture* weapon_texture;
    SDL_Rect weapon_rect;
};

#endif // PLAYER_H
