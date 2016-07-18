#ifndef LEVEL_H
#define LEVEL_H

#include "Application.h"
#include "Tools.h"
#include "AmmoPickup.h"
#include "HealthPickup.h"
#include "Zombie.h"
#include "Demon.h"
#include "Hound.h"
#include "Boss.h"
#include "Projectile.h"

#include <algorithm>
#include <queue>
#include <deque>

/**
* This type is just used to differentiate between ammo pickups
* and weapon pickups.
*/
typedef AmmoPickup WeaponPickup;

/**
* This class loads a level from a file. It renders the level into a single
* texture for drawing and also loads all of the information needed for a level, such
* as the player's starting position, the exit, the enemies and pickups.
*/
class Level
{
public:
    /**
    * This method loads all of the data for a level from a file. It does not
    * render the level, it only parses the file and gathers all of the information
    * needed to render the level.
    */
    void load(const std::string& file_name);

    /**
    * This method renders the level so that it can be drawn to the screen.
    */
    void render();

    /**
    * This method updates all of the enemies in the game.
    */
    void update(Player& player);

    /**
    * This method draws the map and every object in the level to the screen.
    */
    void draw();

    /**
    * This method returns the current file name.
    */
    const std::string& getFileName();

    /**
    * This method gets the title of the currently loaded level.
    */
    const std::string& getTitle();

    /**
    * This method gets the start rect for the player.
    */
    SDL_Rect& getStartRect();

    /**
    * This method returns a vector of rects for every wall in the level.
    */
    const std::vector<SDL_Rect>& getWallRects();

    /**
    * This method returns a vector of every ammo pickup in the level.
    */
    std::vector<AmmoPickup>& getAmmoPickups();

    /**
    * This method returns a vector of every weapon pickup in the level.
    */
    std::vector<WeaponPickup>& getWeaponPickups();

    /**
    * This method returns a vector of every health pickup in the level.
    */
    std::vector<HealthPickup>& getHealthPickups();

    /**
    * This method returns a vector of every enemy in the level.
    */
    std::vector<std::shared_ptr<Enemy>>& getEnemies();

    /**
    * This method returns a vector of all projectiles fired by enemies.
    */
    std::vector<Projectile>& getEnemyProjectiles();

    /**
    * This destructor is needed to destroy the current map texture.
    */
    ~Level();

    /**
    * This method generates the grid used for pathfinding. It simply makes a grid
    * which stores walls and non-walls.
    */
    void generateAIGrid();

    /**
    * This method generates a map of every tile in the grid. Any point in this map
    * can be traced back to start_tile which can be used to create a path.
    */
    void breadthFirstSearch(const SDL_Point& start_tile);

    /**
    * This method returns a list of each tile from end_tile to start_tile.
    */
    std::deque<SDL_Point> getPathToTile(const SDL_Point& end_tile);

private:
    /**
    * This method processes all of the objects in the level, such as weapon pickups
    * and player starting position.
    */
    void processObjects(std::string& object_text);

    /**
    * This method processes all of the layers in the level.
    */
    void processLayers(std::string& layer_text);

    /**
    * This method just clears all of the previous level data such
    * as enemies and pickups.
    */
    void clearPreviousLevelData();

    /**
    * This method checks an individual tile in the AI grid and sees if
    * it is traversable. If it is, and it has not been visited before,
    * it adds it the came_from map.
    */
    void checkTile(std::queue<SDL_Point>& frontier, const int x, const int y, const int orig_x, const int orig_y);

public:
    static const int TILE_SIZE = 50;

private:
    SDL_Texture* map_texture;
    SDL_Rect map_rect;

    // Map data.
    std::string file_name;
    std::string title;
    int width, height;
    std::vector<std::vector<std::string>> map_data;

    // Object data.
    SDL_Rect player_rect;
    std::vector<SDL_Rect> wall_rects;
    std::vector<AmmoPickup> ammo_pickups;
    std::vector<WeaponPickup> weapon_pickups;
    std::vector<HealthPickup> health_pickups;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<Projectile> enemy_projectiles;

    // AI data.
    std::vector<std::vector<char>> grid;
    std::map<std::pair<int, int>, std::pair<int, int>> came_from;
    SDL_Point start_tile;
};

#endif // LEVEL_H
