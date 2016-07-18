#ifndef ENEMY_H
#define ENEMY_H

#include "Application.h"
#include "Tools.h"

#include <deque>

class Level;
class Player;

/**
* This class is a base class for every enemy in the game. It handles health
* and AI.
*/
class Enemy
{
public:
    /**
    * The constructor sets the starting rect of the enemy.
    */
    Enemy(const SDL_Rect& rect, SDL_Texture* texture, const int speed, const int health);

    /**
    * This method needs to know about the level and where the player is. This
    * method handles all collisions and AI (not pathfinding however).
    */
    virtual void update(Level* level, Player& player);

    /**
    * This method draws the enemy to the screen.
    */
    void draw();

    /**
    * This method inflicts damage to the enemy.
    */
    void damage(const int damage);

    /**
    * This method returns whether or not the enemy is dead (has no health).
    */
    bool isDead();

    /**
    * This method gets the rect of the enemy.
    */
    const SDL_Rect& getRect();

protected:
    static const int NODE_SIZE = 10;
    static constexpr float AI_TIME_MINIMUM = 0.3;
    static constexpr float AI_TIME_MAXIMUM = 1.0;

    int speed;
    int health;
    float ai_time, ai_timer = 0.0;
    bool alerted = false;
    bool dead = false;
    int angle = 0;
    bool facing_player = false;

    SDL_Texture* texture;
    SDL_Rect rect, node_rect;
    std::deque<SDL_Point> current_path;
    Tools::FloatVector normal = {0.0, 0.0};

    int x1, y1, x2, y2;
};

#endif // ENEMY_H
