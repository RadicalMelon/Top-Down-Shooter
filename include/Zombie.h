#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "Enemy.h"

class Zombie : public Enemy
{
public:
    Zombie(const SDL_Rect& rect);
    void update(Level* level, Player& player) override;

private:
    static constexpr float ATTACK_TIME = 0.7;

    float attack_timer = 0.0;
    SDL_Rect melee_rect;
};

#endif // ZOMBIE_H
