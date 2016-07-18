#include "Demon.h"
#include "Level.h"
#include "Player.h"

Demon::Demon(const SDL_Rect& rect) : Enemy(rect, Application::getTexture("Resources/Images/Demon.png"), 150, 200)
{
}

void Demon::update(Level* level, Player& player)
{
    Enemy::update(level, player);

    if (facing_player)
    {
        attack_timer += Application::getDeltaTime();
        if (attack_timer >= ATTACK_TIME)
        {
            attack_timer = 0.0;
            if (!Tools::getRandomInt(0, 3))
            {
                level->getEnemyProjectiles().emplace_back(Application::getTexture("Resources/Images/Bullet.png"),
                                                      rect.x + (rect.w / 2), rect.y + (rect.h / 2), angle, 20, 1000, 5);
            }
        }
    }
}
