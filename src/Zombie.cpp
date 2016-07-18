#include "Zombie.h"
#include "Player.h"

Zombie::Zombie(const SDL_Rect& rect) : Enemy(rect, Application::getTexture("Resources/Images/Zombie.png"), 100, 100)
{
    melee_rect.w = 100;
    melee_rect.h = 100;
}

void Zombie::update(Level* level, Player& player)
{
    Enemy::update(level, player);

    melee_rect.x = rect.x - (melee_rect.w / 2);
    melee_rect.y = rect.y - (melee_rect.h / 2);

    if (SDL_HasIntersection(&melee_rect, &player.getRect()))
    {
        attack_timer += Application::getDeltaTime();
        if (attack_timer >= ATTACK_TIME)
        {
            attack_timer = 0.0;
            player.damage(25);
        }
    }
}
