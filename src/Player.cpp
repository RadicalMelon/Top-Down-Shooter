#include "Player.h"

Player::Player() : ammo_counter(Application::getFont("Resources/Fonts/GameFont.ttf", 24), "Ammo: ", 20, 20, false),
                   health_count(Application::getFont("Resources/Fonts/GameFont.ttf", 24), "Health: ", Application::getWindowSize().x - 150, 20, false)
{
    texture = Application::getTexture("Resources/Images/Player.png");
    SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);

    updateAmmoCounter();
    updateHealthCount();

    weapon_texture = Application::getTexture("Resources/Images/Handgun.png");
    SDL_QueryTexture(weapon_texture, nullptr, nullptr, &weapon_rect.w, &weapon_rect.h);
    weapon_rect.x = 20;
    weapon_rect.y = 60;
    weapon_rect.w *= 2;
    weapon_rect.h *= 2;
}

/**
* This method respawns the player by resetting some attributes.
*/
void Player::respawn()
{
    ammo = {
        {Weapon::Handgun,         25},
        {Weapon::Shotgun,          0},
        {Weapon::AssaultRifle,     0},
        {Weapon::Minigun,          0},
        {Weapon::PlasmaRifle,      0}
    };

    health = MAX_HEALTH;

    shooting = false;
    current_weapon = Weapon::Handgun;
    weapon_index = 0;
    weapons = {Weapon::Handgun};

    updateAmmoCounter();
    weapon_texture = Application::getTexture("Resources/Images/Handgun.png");
    SDL_QueryTexture(weapon_texture, nullptr, nullptr, &weapon_rect.w, &weapon_rect.h);
    weapon_rect.x = 20;
    weapon_rect.y = 60;
    weapon_rect.w *= 2;
    weapon_rect.h *= 2;

    updateHealthCount();
}

/**
* This method handles the key presses that are relevant to the player.
* This mostly includes movement.
*/
void Player::handleKeyPresses()
{
    switch (Application::getEvent().key.keysym.sym)
    {
    case SDLK_a:
        movement.x = -SPEED;
        break;
    case SDLK_d:
        movement.x = SPEED;
        break;
    case SDLK_w:
        movement.y = -SPEED;
        break;
    case SDLK_s:
        movement.y = SPEED;
        break;
    default:
        break;
    }
}

/**
* This method handles the key releases that are relevant to the player.
* This mostly includes movement.
*/
void Player::handleKeyReleases()
{
    switch (Application::getEvent().key.keysym.sym)
    {
    case SDLK_a:
    case SDLK_d:
        movement.x = 0;
        break;
    case SDLK_w:
    case SDLK_s:
        movement.y = 0;
        break;
    default:
        break;
    }
}

/**
* This method updates the player. It moves the player and also stops the player
* from moving solid objects.
*/
void Player::update(const std::vector<SDL_Rect>& solids)
{
    // Collisions for the X axis.
    rect.x += std::round(movement.x * Application::getDeltaTime());
    for (const auto& solid : solids)
    {
        if (SDL_HasIntersection(&solid, &rect))
        {
            // If the player is moving to the right.
            if (movement.x > 0)
            {
                rect.x = solid.x - rect.w;
            }
            // If the player is moving to the left.
            else if (movement.x < 0)
            {
                rect.x = solid.x + solid.w;
            }
        }
    }

    // Collisions for the Y axis.
    rect.y += std::round(movement.y * Application::getDeltaTime());
    for (const auto& solid : solids)
    {
        if (SDL_HasIntersection(&solid, &rect))
        {
            // If the player is moving down.
            if (movement.y > 0)
            {
                rect.y = solid.y - rect.h;
            }
            // If the player is moving up.
            else if (movement.y < 0)
            {
                rect.y = solid.y + solid.h;
            }
        }
    }

    // Center the camera on the player.
    Application::getCamera().x = (Application::getWindowSize().x / 2) - rect.x;
    Application::getCamera().y = (Application::getWindowSize().y / 2) - rect.y;

    // Make sure the player faces the mouse.
    angle = Tools::angleBetweenPoints(rect.x + (rect.w / 2), rect.y + (rect.h / 2),
                                      Application::getMousePosition().x - Application::getCamera().x,
                                      Application::getMousePosition().y - Application::getCamera().y) - 180;

    shoot_delay += Application::getDeltaTime();
}

/**
* This method draws the player to the screen.
*/
void Player::draw()
{
    SDL_Rect draw_rect = Application::convertToCameraView(rect);
    SDL_RenderCopyEx(Application::getRenderer(), texture, nullptr, &draw_rect, angle, nullptr, SDL_FLIP_NONE);
    ammo_counter.draw();
    SDL_RenderCopy(Application::getRenderer(), weapon_texture, nullptr, &weapon_rect);
    health_count.draw();
}

/**
* This method sets the player's rect. This is used to set the player's
* starting position in each level.
*/
void Player::setRect(SDL_Rect& rect)
{
    this->rect.x = rect.x;
    this->rect.y = rect.y;
    this->rect.x += rect.w / 2;
    this->rect.y += rect.h / 2;
    this->rect.x -= this->rect.w / 2;
    this->rect.y -= this->rect.h / 2;
}

/**
* This method gets the player's rect.
*/
SDL_Rect& Player::getRect()
{
    return rect;
}

/**
* This method changes the player's current weapon.
*/
void Player::changeWeapon()
{
    weapon_index += Application::getEvent().wheel.y;
    if (weapon_index < 0)
    {
        weapon_index = weapons.size() - 1;
    }
    else if (weapon_index > (static_cast<int>(weapons.size()) - 1))
    {
        weapon_index = 0;
    }
    current_weapon = weapons[weapon_index];

    // Update the ammo counter on the HUD.
    updateAmmoCounter();

    switch (current_weapon)
    {
    case Weapon::Handgun:
        weapon_texture = Application::getTexture("Resources/Images/Handgun.png");
        break;
    case Weapon::Shotgun:
        weapon_texture = Application::getTexture("Resources/Images/Shotgun.png");
        break;
    case Weapon::AssaultRifle:
        weapon_texture = Application::getTexture("Resources/Images/AssaultRifle.png");
        break;
    case Weapon::Minigun:
        weapon_texture = Application::getTexture("Resources/Images/Minigun.png");
        break;
    case Weapon::PlasmaRifle:
        weapon_texture = Application::getTexture("Resources/Images/PlasmaRifle.png");
        break;
    default:
        break;
    }
    SDL_QueryTexture(weapon_texture, nullptr, nullptr, &weapon_rect.w, &weapon_rect.h);
    weapon_rect.w *= 2;
    weapon_rect.h *= 2;
}

/**
* This method gets the player's current weapon.
*/
const Weapon& Player::getWeapon()
{
    return current_weapon;
}

/**
* This method returns if you can shoot your weapon right now.
*/
bool Player::canShoot()
{
    // If we're shooting and we have enough ammo.
    if (shooting && (ammo[current_weapon] > 0))
    {
        // And if we have waited long enough.
        if (shoot_delay > WEAPON_DELAYS.at(current_weapon))
        {
            ammo[current_weapon]--;
            updateAmmoCounter();
            shoot_delay = 0.0;
            return true;
        }
    }
    return false;
}

/**
* This method tells the player to start shooting.
*/
void Player::startShooting()
{
    shooting = true;
}

/**
* This method tells the player to stop shooting.
*/
void Player::stopShooting()
{
    shooting = false;
}

/**
* This method returns the player's centre.
*/
SDL_Point Player::getCentre()
{
    return {rect.x + (rect.w / 2), rect.y + (rect.h / 2)};
}

/**
* This method adds ammo for a certain weapon. It returns whether or not
* ammo has been added.
*/
bool Player::addAmmo(const Weapon& weapon, const int ammo)
{
    if ((this->ammo[weapon] + ammo) >= (MAX_AMMO.at(weapon) + ammo))
    {
        return false;
    }
    else
    {
        this->ammo[weapon] += ammo;
        if (this->ammo[weapon] >= MAX_AMMO.at(weapon))
        {
            this->ammo[weapon] = MAX_AMMO.at(weapon);
        }
        if (weapon == current_weapon)
        {
            updateAmmoCounter();
        }
        return true;
    }
}

/**
* This method returns whether or not the player has a weapon in their inventory.
*/
bool Player::hasWeapon(const Weapon& weapon)
{
    return std::find(weapons.begin(), weapons.end(), weapon) != weapons.end();
}

/**
* This method adds a weapon to the player's inventory.
*/
void Player::addWeapon(const Weapon& weapon)
{
    weapons.push_back(weapon);
}

/**
* This method adds to the player's health.
*/
bool Player::addHealth(const int health)
{
    if ((this->health + health) >= (MAX_HEALTH + health))
    {
        return false;
    }
    else
    {
        this->health += health;
        if (this->health >= MAX_HEALTH)
        {
            this->health = MAX_HEALTH;
        }
        updateHealthCount();
        return true;
    }
}

/**
* This method damages the player.
*/
void Player::damage(const int damage)
{
    health -= damage;
    updateHealthCount();
}

/**
* This method gets the player's angle.
*/
int Player::getAngle()
{
    return angle;
}

/**
* This method checks if the player is dead.
*/
bool Player::isDead()
{
    return health <= 0;
}

/**
* This method updates the ammo counter on the HUD.
*/
void Player::updateAmmoCounter()
{
    std::stringstream ss;
    ss << "Ammo: " << ammo[current_weapon];
    ammo_counter.setText(ss.str());
}

/**
* This method updates the health count on the HUD.
*/
void Player::updateHealthCount()
{
    std::stringstream ss;
    ss << "Health: " << health;
    health_count.setText(ss.str());
}
