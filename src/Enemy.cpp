#include "Enemy.h"
#include "Level.h"
#include "Player.h"

/**
* The constructor sets the starting rect of the enemy.
*/
Enemy::Enemy(const SDL_Rect& rect, SDL_Texture* texture, const int speed, const int health)
{
    this->texture = texture;
    SDL_QueryTexture(this->texture, nullptr, nullptr, &this->rect.w, &this->rect.h);

    // Center the enemy on its tile.
    this->rect.x = rect.x + (rect.w / 2);
    this->rect.y = rect.y + (rect.h / 2);
    this->rect.x -= this->rect.w / 2;
    this->rect.y -= this->rect.h / 2;

    this->speed = speed;
    this->health = health;

    // Set the delay needed for getting the next path to the player.
    // This is random to stop every enemy from switching direction at once,
    // it adds some individuality to the enemies.
    ai_time = Tools::getRandomFloat(AI_TIME_MINIMUM, AI_TIME_MAXIMUM);
}

/**
* This method needs to know about the level and where the player is. This
* method handles all collisions and AI (not pathfinding however).
*/
void Enemy::update(Level* level, Player& player)
{
    x1 = rect.x + (rect.w / 2);
    y1 = rect.y + (rect.h / 2);
    x2 = player.getRect().x + (player.getRect().w / 2);
    y2 = player.getRect().y + (player.getRect().h / 2);

    if (!alerted)
    {
        // If there are no walls blocking the enemy's line of sight, then it can see the player.
        if (std::all_of(level->getWallRects().begin(),
                        level->getWallRects().end(),
                        [&](const SDL_Rect& wall){return !SDL_IntersectRectAndLine(&wall, &x1, &y1, &x2, &y2);}))
        {
            alerted = true;
        }

    }
    else
    {
        ai_timer += Application::getDeltaTime();
        if (current_path.empty() || ai_timer >= ai_time)
        {
            ai_timer = 0.0;
            current_path = level->getPathToTile({rect.x / level->TILE_SIZE, rect.y / level->TILE_SIZE});
            current_path.pop_back();

            node_rect = {(current_path.back().x * level->TILE_SIZE) + (level->TILE_SIZE / 2) - (NODE_SIZE / 2),
                         (current_path.back().y * level->TILE_SIZE) + (level->TILE_SIZE / 2) - (NODE_SIZE / 2),
                         NODE_SIZE, NODE_SIZE};
        }

        // If the center of the enemy is in the next node in the path.
        if (((rect.x + (rect.w / 2)) > node_rect.x) &&
            ((rect.x + (rect.w / 2)) < node_rect.x + node_rect.w) &&
            ((rect.y + (rect.h / 2)) > node_rect.y) &&
            ((rect.y + (rect.h / 2)) < node_rect.y + node_rect.h))
        {
            current_path.pop_back();

            node_rect = {(current_path.back().x * level->TILE_SIZE) + (level->TILE_SIZE / 2) - (NODE_SIZE / 2),
                         (current_path.back().y * level->TILE_SIZE) + (level->TILE_SIZE / 2) - (NODE_SIZE / 2),
                         NODE_SIZE, NODE_SIZE};
        }

        if (current_path.empty())
        {
            normal.x = 0;
            normal.y = 0;
        }
        else
        {
            // Make sure that the enemy moves towards the next node in the path. If it misses the node,
            // it will turn around and move towards it.
            angle = Tools::angleBetweenPoints(rect.x + (rect.w / 2), rect.y + (rect.h / 2),
                                              node_rect.x + (node_rect.w / 2), node_rect.y + (node_rect.h / 2)) - 180;

            // Make sure that the enemy moves towards the next node in the path.
            normal.x = std::cos(angle * 0.0174533);
            normal.y = std::sin(angle * 0.0174533);
            normal = Tools::normalizeVector(normal);
            normal.x *= speed;
            normal.y *= speed;

            // This means that if the player is in the line of sights, it will face towards the player, but will still
            // move towards the node because we are not changing any movement variables here.
            if (std::all_of(level->getWallRects().begin(),
                            level->getWallRects().end(),
                            [&](const SDL_Rect& wall){return !SDL_IntersectRectAndLine(&wall, &x1, &y1, &x2, &y2);}))
            {
                if (!facing_player)
                {
                    facing_player = true;
                }
                angle = Tools::angleBetweenPoints(rect.x + (rect.w / 2), rect.y + (rect.h / 2),
                                                  player.getRect().x + (player.getRect().w / 2), player.getRect().y + (player.getRect().h / 2)) - 180;
            }
            else
            {
                if (facing_player)
                {
                    facing_player = false;
                }
            }
        }

        // This just constructs a list of every solid in the game for use with collision.
        std::vector<SDL_Rect> solids = level->getWallRects();
        solids.push_back(player.getRect());
        for (auto& enemy : level->getEnemies())
        {
            // This is to stop the enemy from colliding with itself.
            if (!SDL_RectEquals(&rect, &enemy->getRect()))
            {
                solids.push_back(enemy->getRect());
            }
        }

        // Collisions for the X axis.
        rect.x += std::round(normal.x * Application::getDeltaTime());
        for (const auto& solid : solids)
        {
            if (SDL_HasIntersection(&solid, &rect))
            {
                // If the enemy is moving to the right.
                if (normal.x > 0)
                {
                    rect.x = solid.x - rect.w;
                }
                // If the enemy is moving to the left.
                else if (normal.x < 0)
                {
                    rect.x = solid.x + solid.w;
                }
            }
        }

        // Collisions for the Y axis.
        rect.y += std::round(normal.y * Application::getDeltaTime());
        for (const auto& solid : solids)
        {
            if (SDL_HasIntersection(&solid, &rect))
            {
                // If the enemy s moving down.
                if (normal.y > 0)
                {
                    rect.y = solid.y - rect.h;
                }
                // If the enemy is moving up.
                else if (normal.y < 0)
                {
                    rect.y = solid.y + solid.h;
                }
            }
        }
    }
}

/**
* This method draws the enemy to the screen.
*/
void Enemy::draw()
{
    SDL_Rect draw_rect = Application::convertToCameraView(rect);
    SDL_RenderCopyEx(Application::getRenderer(), texture, nullptr, &draw_rect, angle, nullptr, SDL_FLIP_NONE);
}

/**
* This method inflicts damage to the enemy.
*/
void Enemy::damage(const int damage)
{
    health -= damage;
    if (health <= 0)
    {
        dead = true;
    }
}

/**
* This method returns whether or not the enemy is dead (has no health).
*/
bool Enemy::isDead()
{
    return dead;
}

/**
* This method gets the rect of the enemy.
*/
const SDL_Rect& Enemy::getRect()
{
    return rect;
}
