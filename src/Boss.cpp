#include "Boss.h"
#include "Player.h"
#include "Level.h"

Boss::Boss(const SDL_Rect& rect) : Enemy(rect, Application::getTexture("Resources/Images/Boss.png"), 250, 500)
{
}

void Boss::update(Level* level, Player& player)
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
                for (int i = 0; i < 3; i++)
                {
                    level->getEnemyProjectiles().emplace_back(Application::getTexture("Resources/Images/PlasmaBall.png"),
                                                          rect.x + (rect.w / 2), rect.y + (rect.h / 2), angle, 20, 1000, 25);
                }
            }
        }
    }
}

