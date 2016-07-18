#include "GameState.h"

void GameState::startUp()
{
    level.load("Resources/Levels/1.lvl");
    level.render();
    player.setRect(level.getStartRect());
    level.generateAIGrid();
    level.breadthFirstSearch({player.getCentre().x / level.TILE_SIZE, player.getCentre().y / level.TILE_SIZE});

    updateWindowTitle();
}

void GameState::handleEvents()
{
    if (Application::getEvent().type == SDL_KEYDOWN)
    {
        if (Application::getEvent().key.keysym.sym == SDLK_SPACE)
        {
            if (level.getTitle() == "Test Level")
            {
                level.load("Resources/Levels/2.lvl");
            }
            else if (level.getTitle() == "Second Test Level")
            {
                level.load("Resources/Levels/1.lvl");
            }

            if (!projectiles.empty())
            {
                projectiles.clear();
            }

            level.render();
            player.setRect(level.getStartRect());
            level.generateAIGrid();
            level.breadthFirstSearch({player.getCentre().x / level.TILE_SIZE, player.getCentre().y / level.TILE_SIZE});

            updateWindowTitle();
        }
        else if (Application::getEvent().key.keysym.sym == SDLK_ESCAPE)
        {
            Application::quit();
        }
        else
        {
            player.handleKeyPresses();
        }
    }
    else if (Application::getEvent().type == SDL_KEYUP)
    {
        player.handleKeyReleases();
    }
    else if (Application::getEvent().type == SDL_MOUSEWHEEL)
    {
        player.changeWeapon();
    }
    else if (Application::getEvent().type == SDL_MOUSEBUTTONDOWN)
    {
        player.startShooting();
    }
    else if (Application::getEvent().type == SDL_MOUSEBUTTONUP)
    {
        player.stopShooting();
    }
}

void GameState::update()
{
    // Create a list of every solid object in the game for the player to collide with.
    solids = level.getWallRects();
    for (auto& enemy : level.getEnemies())
    {
        solids.push_back(enemy->getRect());
    }
    player.update(solids);

    // If the player can shoot, a different projectile will be spawned at their location depending
    // on what weapon the player has equipped.
    if (player.canShoot())
    {
        switch (player.getWeapon())
        {
        case Weapon::Handgun:
            projectiles.emplace_back(Application::getTexture("Resources/Images/Bullet.png"),
                                     player.getCentre().x, player.getCentre().y, player.getAngle(), 20, 1000, 5);
            break;
        case Weapon::Shotgun:
            {
                for (int i = 0; i < 5; i++)
                {
                    projectiles.emplace_back(Application::getTexture("Resources/Images/BallBearing.png"),
                                             player.getCentre().x, player.getCentre().y, player.getAngle(), 20, 1000, 25);
                }
            }
            break;
        case Weapon::AssaultRifle:
            projectiles.emplace_back(Application::getTexture("Resources/Images/AssaultRifleBullet.png"),
                                     player.getCentre().x, player.getCentre().y, player.getAngle(), 20, 1000, 3);
            break;
        case Weapon::Minigun:
            projectiles.emplace_back(Application::getTexture("Resources/Images/MinigunBullet.png"),
                                     player.getCentre().x, player.getCentre().y, player.getAngle(), 20, 1000, 20);
            break;
        case Weapon::PlasmaRifle:
            projectiles.emplace_back(Application::getTexture("Resources/Images/PlasmaBall.png"),
                                     player.getCentre().x, player.getCentre().y, player.getAngle(), 20, 1000, 2);
            break;
        default:
            break;
        }
    }

    // Move all player projectiles.
    for (auto& projectile : projectiles)
    {
        projectile.update();
    }

    // Remove player projectile if it hits a wall.
    for (auto& wall : level.getWallRects())
    {
        for (auto projectile = projectiles.begin(); projectile != projectiles.end();)
        {
            if (SDL_HasIntersection(&wall, &projectile->getRect()))
            {
                projectiles.erase(projectile);
            }
            else
            {
                projectile++;
            }
        }
    }

    for (auto pickup = level.getAmmoPickups().begin(); pickup != level.getAmmoPickups().end();)
    {
        if (SDL_HasIntersection(&pickup->getRect(), &player.getRect()))
        {
            if (player.addAmmo(pickup->getWeapon(), pickup->getAmmo()))
            {
                level.getAmmoPickups().erase(pickup);
            }
            else
            {
                pickup++;
            }
        }
        else
        {
            pickup++;
        }
    }

    for (auto pickup = level.getWeaponPickups().begin(); pickup != level.getWeaponPickups().end();)
    {
        if (SDL_HasIntersection(&pickup->getRect(), &player.getRect()))
        {
            if (!player.hasWeapon(pickup->getWeapon()))
            {
                player.addWeapon(pickup->getWeapon());
                player.addAmmo(pickup->getWeapon(), pickup->getAmmo());
                level.getWeaponPickups().erase(pickup);
            }
            else if (player.addAmmo(pickup->getWeapon(), pickup->getAmmo()))
            {
                level.getWeaponPickups().erase(pickup);
            }
            else
            {
                pickup++;
            }
        }
        else
        {
            pickup++;
        }
    }

    for (auto pickup = level.getHealthPickups().begin(); pickup != level.getHealthPickups().end();)
    {
        if (SDL_HasIntersection(&pickup->getRect(), &player.getRect()))
        {
            if (player.addHealth(pickup->getHealth()))
            {
                level.getHealthPickups().erase(pickup);
            }
            else
            {
                pickup++;
            }
        }
        else
        {
            pickup++;
        }
    }

    for (auto enemy = level.getEnemies().begin(); enemy != level.getEnemies().end();)
    {
        for (auto projectile = projectiles.begin(); projectile != projectiles.end();)
        {
            if (SDL_HasIntersection(&enemy->get()->getRect(), &projectile->getRect()))
            {
                enemy->get()->damage(projectile->getDamage());
                projectiles.erase(projectile);
            }
            else
            {
                projectile++;
            }
        }
        if (enemy->get()->isDead())
        {
            level.getEnemies().erase(enemy);
        }
        else
        {
            enemy++;
        }
    }

    level.update(player);

    fps_timer += Application::getDeltaTime();
    if (fps_timer >= TITLE_UPDATE_TIME)
    {
        fps_timer = 0.0;
        updateWindowTitle();
    }

    ai_timer += Application::getDeltaTime();
    if (ai_timer >= BFS_UPDATE_TIME)
    {
        ai_timer = 0.0;
        level.breadthFirstSearch({player.getCentre().x / level.TILE_SIZE, player.getCentre().y / level.TILE_SIZE});
    }

    if (player.isDead())
    {
        player.respawn();
        level.load(level.getFileName());
        player.setRect(level.getStartRect());
        level.render();
        level.generateAIGrid();
        level.breadthFirstSearch({player.getCentre().x / level.TILE_SIZE, player.getCentre().y / level.TILE_SIZE});
    }
}

void GameState::draw()
{
    level.draw();
    for (auto& projectile : projectiles)
    {
        projectile.draw();
    }
    player.draw();
}

void GameState::shutDown()
{
}

void GameState::updateWindowTitle()
{
    std::stringstream ss;
    ss << "Top Down Shooter" << " - " << level.getTitle() << " - " << "FPS: " << Application::getFrameRate();
    Application::setWindowTitle(ss.str());
}
