#ifndef DEMON_H
#define DEMON_H

#include "Enemy.h"

class Demon : public Enemy
{
public:
    Demon(const SDL_Rect& rect);
    void update(Level* level, Player& player) override;

private:
    static constexpr float ATTACK_TIME = 1.0;

    float attack_timer = 0.0;
};

#endif // DEMON_H
