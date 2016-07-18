#ifndef HOUND_H
#define HOUND_H

#include "Enemy.h"

class Hound : public Enemy
{
public:
    Hound(const SDL_Rect& rect);
    void update(Level* level, Player& player) override;

private:
    static constexpr float ATTACK_TIME = 1.0;

    float attack_timer = 0.0;
    SDL_Rect melee_rect;
};

#endif // HOUND_H
