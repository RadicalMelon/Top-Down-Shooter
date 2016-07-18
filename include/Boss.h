#ifndef BOSS_H
#define BOSS_H

#include "Enemy.h"

class Boss : public Enemy
{
public:
    Boss(const SDL_Rect& rect);
    void update(Level* level, Player& player) override;

private:
    static constexpr float ATTACK_TIME = 1.0;

    float attack_timer = 0.0;
};

#endif // BOSS_H
