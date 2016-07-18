#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Application.h"
#include "Level.h"
#include "Player.h"
#include "Projectile.h"

class GameState : public Application::BaseState
{
public:
    void startUp() override;
    void handleEvents() override;
    void update() override;
    void draw() override;
    void shutDown() override;

private:
    void updateWindowTitle();

private:
    static constexpr float TITLE_UPDATE_TIME = 1.0;
    static constexpr float BFS_UPDATE_TIME = 0.5;

    Level level;
    Player player;
    std::vector<Projectile> projectiles;
    float fps_timer = 0.0;
    float ai_timer = 0.0;

    std::vector<SDL_Rect> solids;
};

#endif // GAMESTATE_H
