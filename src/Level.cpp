#include "Level.h"

/**
* This method loads all of the data for a level from a file. It does not
* render the level, it only parses the file and gathers all of the information
* needed to render the level.
*/
void Level::load(const std::string& file_name)
{
    clearPreviousLevelData();
    this->file_name = file_name;

    DEBUG("Loading level: " << this->file_name);

    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(this->file_name.c_str()) != tinyxml2::XML_SUCCESS)
    {
        SDL_SetError(doc.ErrorName());
        throw Application::Error::XML;
    }

    for (tinyxml2::XMLNode* node = doc.FirstChild(); node; node = node->NextSibling())
    {
        std::string node_name = node->ToElement()->Name();
        if (node_name == "about")
        {
            for (tinyxml2::XMLNode* about_node = node->FirstChild(); about_node; about_node = about_node->NextSibling())
            {
                std::string name = about_node->ToElement()->Name();
                std::string text = about_node->ToElement()->GetText();
                if (name == "title")
                {
                    title = text;
                    DEBUG("Level title: " << title);
                }
                else if (name == "width")
                {
                    width = atoi(text.c_str());
                    DEBUG("Level width: " << width);
                }
                else if (name == "height")
                {
                    height = atoi(text.c_str());
                    DEBUG("Level height: " << height);
                }
            }
        }

        else if (node_name == "level")
        {
            for (tinyxml2::XMLNode* level_node = node->FirstChild(); level_node; level_node = level_node->NextSibling())
            {
                std::string level_node_name = level_node->ToElement()->Name();
                if (level_node_name == "layers")
                {
#ifdef DEBUG_ENABLED
                    int layer_num = 0;
#endif // DEBUG_ENABLED
                    for (tinyxml2::XMLNode* layer_node = level_node->FirstChild(); layer_node; layer_node = layer_node->NextSibling())
                    {
                        std::string layer_node_name = layer_node->ToElement()->Name();
                        if (layer_node_name == "layer")
                        {
                            std::string layer_text = layer_node->ToElement()->GetText();
                            processLayers(layer_text);
                            DEBUG("Layer " << ++layer_num << ":" << std::endl << layer_text);
                        }
                    }
                }

                if (level_node_name == "objects")
                {
                    std::string object_text = level_node->ToElement()->GetText();
                    processObjects(object_text);
                }
            }
        }
    }
}

/**
* This method renders the level so that it can be drawn to the screen.
*/
void Level::render()
{
    // If the texture has been loaded before, destroy it.
    if (map_texture != nullptr)
    {
        SDL_DestroyTexture(map_texture);
    }

    // Create a map texture and set it as the render target.
    map_texture = SDL_CreateTexture(Application::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width * TILE_SIZE, height * TILE_SIZE);
    map_rect = {0, 0, width * TILE_SIZE, height * TILE_SIZE};
    SDL_SetRenderTarget(Application::getRenderer(), map_texture);
    SDL_RenderClear(Application::getRenderer());

    SDL_Rect tile_rect = {0, 0, TILE_SIZE, TILE_SIZE};

    // This is so every layer in the map gets drawn.
    for (const auto& layer : map_data)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // Ignore things that aren't tiles.
                if (layer[y][x] == '0' || layer[y][x] == '\0')
                {
                    continue;
                }

                // Get the string for the image of the tile.
                std::string image_file;
                image_file += "Resources/Images/Tiles/";
                image_file += layer[y][x];
                image_file += ".png";

                // Load and render the tile.
                tile_rect.y = y * TILE_SIZE;
                tile_rect.x = x * TILE_SIZE;
                SDL_RenderCopy(Application::getRenderer(), Application::getTexture(image_file), nullptr, &tile_rect);
            }
        }
    }

    SDL_RenderPresent(Application::getRenderer());
    SDL_SetRenderTarget(Application::getRenderer(), nullptr);
}

/**
* This method updates all of the enemies in the game.
*/
void Level::update(Player& player)
{
    for (auto& enemy : enemies)
    {
        enemy->update(this, player);
    }

    for (auto& projectile : enemy_projectiles)
    {
        projectile.update();
    }

    for (auto& wall : wall_rects)
    {
        for (auto projectile = enemy_projectiles.begin(); projectile != enemy_projectiles.end();)
        {
            if (SDL_HasIntersection(&wall, &projectile->getRect()))
            {
                enemy_projectiles.erase(projectile);
            }
            else
            {
                projectile++;
            }
        }
    }

    for (auto projectile = enemy_projectiles.begin(); projectile != enemy_projectiles.end();)
    {
        if (SDL_HasIntersection(&projectile->getRect(), &player.getRect()))
        {
            player.damage(projectile->getDamage());
            enemy_projectiles.erase(projectile);
        }
        else
        {
            projectile++;
        }
    }
}

/**
* This method draws the map and every object in the level to the screen.
*/
void Level::draw()
{
    SDL_Rect draw_rect = Application::convertToCameraView(map_rect);
    SDL_RenderCopy(Application::getRenderer(), map_texture, nullptr, &draw_rect);

    for (auto& pickup : ammo_pickups)
    {
        pickup.draw();
    }

    for (auto& pickup : weapon_pickups)
    {
        pickup.draw();
    }

    for (auto& pickup : health_pickups)
    {
        pickup.draw();
    }

    for (auto& enemy : enemies)
    {
        enemy->draw();
    }

    for (auto& projectile : enemy_projectiles)
    {
        projectile.draw();
    }
}

/**
* This method returns the current file name.
*/
const std::string& Level::getFileName()
{
    return file_name;
}

/**
* This method gets the title level.
*/
const std::string& Level::getTitle()
{
    return title;
}

/**
* This method gets the start rect for the player.
*/
SDL_Rect& Level::getStartRect()
{
    return player_rect;
}

/**
* This method returns a vector of rects for every wall in the level.
*/
const std::vector<SDL_Rect>& Level::getWallRects()
{
    return wall_rects;
}

/**
* This method returns a vector of every ammo pickup in the level.
*/
std::vector<AmmoPickup>& Level::getAmmoPickups()
{
    return ammo_pickups;
}

/**
* This method returns a vector of every weapon pickup in the level.
*/
std::vector<WeaponPickup>& Level::getWeaponPickups()
{
    return weapon_pickups;
}

/**
* This method returns a vector of every health pickup in the level.
*/
std::vector<HealthPickup>& Level::getHealthPickups()
{
    return health_pickups;
}

/**
* This method returns a vector of every enemy in the level.
*/
std::vector<std::shared_ptr<Enemy>>& Level::getEnemies()
{
    return enemies;
}

std::vector<Projectile>& Level::getEnemyProjectiles()
{
    return enemy_projectiles;
}

/**
* This destructor is needed to destroy the current map texture.
*/
Level::~Level()
{
    // We need to destroy the map texture because it is created by this class and
    // is not loaded in the Application namespace.
    SDL_DestroyTexture(map_texture);
}

/**
* This method processes all of the objects in the level, such as weapon pickups
* and player starting position.
*/
void Level::processObjects(std::string& object_text)
{
    // Remove all white space from the object data because it may be indented, and spaces aren't needed.
    object_text.erase(std::remove(object_text.begin(), object_text.end(), ' '), object_text.end());

    // Remove newlines from the beginning and end of the object data, as they would mess with splitting by lines.
    object_text.erase(object_text.begin());
    object_text.pop_back();

    DEBUG("Objects: " << std::endl << object_text);

    // Split the object data by line.
    auto object_data = Tools::splitString(object_text, '\n');

    SDL_Rect object_rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            object_rect.y = y * TILE_SIZE;
            object_rect.x = x * TILE_SIZE;

            // Process object data.
            switch (object_data[y][x])
            {
            case 'A':
                player_rect = object_rect;
                break;
            case 'B':
                {
                    auto zombie = std::make_shared<Zombie>(object_rect);
                    enemies.push_back(zombie);
                }
                break;
            case 'C':
                {
                    auto demon = std::make_shared<Demon>(object_rect);
                    enemies.push_back(demon);
                }
                break;
            case 'D':
                {
                    auto hound = std::make_shared<Hound>(object_rect);
                    enemies.push_back(hound);
                }
                break;
            case 'E':
                {
                    auto boss = std::make_shared<Boss>(object_rect);
                    enemies.push_back(boss);
                }
                break;
            case 'F':
                ammo_pickups.emplace_back(Application::getTexture("Resources/Images/HandgunAmmoPickup.png"), object_rect, 10, Weapon::Handgun);
                break;
            case 'G':
                ammo_pickups.emplace_back(Application::getTexture("Resources/Images/ShotgunAmmoPickup.png"), object_rect, 20, Weapon::Shotgun);
                break;
            case 'H':
                ammo_pickups.emplace_back(Application::getTexture("Resources/Images/AssaultRifleAmmoPickup.png"), object_rect, 20, Weapon::AssaultRifle);
                break;
            case 'I':
                ammo_pickups.emplace_back(Application::getTexture("Resources/Images/MinigunAmmoPickup.png"), object_rect, 20, Weapon::Minigun);
                break;
            case 'J':
                ammo_pickups.emplace_back(Application::getTexture("Resources/Images/PlasmaAmmoPickup.png"), object_rect, 20, Weapon::PlasmaRifle);
                break;
            case 'K':
                weapon_pickups.emplace_back(Application::getTexture("Resources/Images/Shotgun.png"), object_rect, 4, Weapon::Shotgun);
                break;
            case 'L':
                weapon_pickups.emplace_back(Application::getTexture("Resources/Images/AssaultRifle.png"), object_rect, 25, Weapon::AssaultRifle);
                break;
            case 'M':
                weapon_pickups.emplace_back(Application::getTexture("Resources/Images/Minigun.png"), object_rect, 30, Weapon::Minigun);
                break;
            case 'N':
                weapon_pickups.emplace_back(Application::getTexture("Resources/Images/PlasmaRifle.png"), object_rect, 10, Weapon::PlasmaRifle);
                break;
            case 'O':
                health_pickups.emplace_back(Application::getTexture("Resources/Images/Stimpack.png"), object_rect, 10);
                break;
            case 'P':
                health_pickups.emplace_back(Application::getTexture("Resources/Images/Medkit.png"), object_rect, 25);
                break;
            default:
                break;
            }
        }
    }
}

/**
* This method processes all of the layers in the level.
*/
void Level::processLayers(std::string& layer_text)
{
    // Remove all white space from the layer data because it may be indented, and spaces aren't needed.
    layer_text.erase(std::remove(layer_text.begin(), layer_text.end(), ' '), layer_text.end());

    // Remove newlines from the beginning and end of the layer data, as they would mess with splitting by lines.
    layer_text.erase(layer_text.begin());
    layer_text.pop_back();

    // Split the layer data by line.
    auto layer_data = Tools::splitString(layer_text, '\n');

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (layer_data[y][x] == '2')
            {
                // Add walls.
                wall_rects.push_back({x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE});
            }
        }
    }

    map_data.push_back(layer_data);
}

/**
* This method just clears all of the previous level data such
* as enemies and pickups.
*/
void Level::clearPreviousLevelData()
{
    // If loading a new level, clear all of the previous map data.
    if (!map_data.empty())
    {
        map_data.clear();
    }

    // If loading a new level, clear all of the previous wall locations.
    if (!wall_rects.empty())
    {
        wall_rects.clear();
    }

    // If loading a new level, clear all of the previous ammo pickups.
    if (!ammo_pickups.empty())
    {
        ammo_pickups.clear();
    }

    // If loading a new level, clear all of the previous weapon pickups.
    if (!weapon_pickups.empty())
    {
        weapon_pickups.clear();
    }

    // If loading a new level, clear all of the previous enemies.
    if (!enemies.empty())
    {
        enemies.clear();
    }

    if (!enemy_projectiles.empty())
    {
        enemy_projectiles.clear();
    }

    if (!health_pickups.empty())
    {
        health_pickups.clear();
    }
}

/**
* This method generates the grid used for pathfinding. It simply makes a grid
* which stores walls and non-walls.
*/
void Level::generateAIGrid()
{
    // Clear the previous AI grid so that it doesn't mess with the new one.
    if (!grid.empty())
    {
        grid.clear();
    }

    // Set the grid to a certain width and height so that we don't have to use
    // push_back or any methods like that. We can access every part of the grid directly.
    grid = std::vector<std::vector<char>>(height, std::vector<char>(width));

    // To catch every wall in the map, we have to look through every layer.
    for (auto& layer : map_data)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // If the current tile is a wall in the layer or the grid.
                if (layer[y][x] == '2' || grid[y][x] == '2')
                {
                    // Set it in the grid.
                    grid[y][x] = '2';
                }
                // If the current tile is not a wall, we can set it to a '1', which
                // represents a non-traversable tile.
                else
                {
                    grid[y][x] = '1';
                }
            }
        }
    }
}

/**
* This method generates a map of every tile in the grid. Any point in this map
* can be traced back to start_tile which can be used to create a path.
*/
void Level::breadthFirstSearch(const SDL_Point& start_tile)
{
    this->start_tile = start_tile;
    std::queue<SDL_Point> frontier;
    frontier.push(this->start_tile);

    // If a this method has been run before, the map of paths has to be cleared so that it doesn't interfere with the
    // new map of paths.
    if (!came_from.empty())
    {
        came_from.clear();
    }

    // Every path found by the algorithm starts here, so we set it to itself.
    came_from[std::make_pair(this->start_tile.x, this->start_tile.y)] = std::make_pair(this->start_tile.x, this->start_tile.y);

    SDL_Point current;
    while (!frontier.empty())
    {
        current = frontier.front();

        // Check every tile above, below and to the sides of the current tile.
        checkTile(frontier, current.x + 1, current.y, current.x, current.y);
        checkTile(frontier, current.x - 1, current.y, current.x, current.y);
        checkTile(frontier, current.x, current.y + 1, current.x, current.y);
        checkTile(frontier, current.x, current.y - 1, current.x, current.y);

        // Check every tile diagonally to the current tile, to allow for diagonal pathfinding.
        checkTile(frontier, current.x + 1, current.y - 1, current.x, current.y);
        checkTile(frontier, current.x + 1, current.y + 1, current.x, current.y);
        checkTile(frontier, current.x - 1, current.y - 1, current.x, current.y);
        checkTile(frontier, current.x - 1, current.y + 1, current.x, current.y);

        frontier.pop();
    }
}

/**
* This method returns a list of each tile from end_tile to start_tile.
*/
std::deque<SDL_Point> Level::getPathToTile(const SDL_Point& end_tile)
{
    std::deque<SDL_Point> path;

    // Tiles are stored as their 2D co-ordinates in the map (not as resolution co-ordinates.)
    std::pair<int, int> current_tile(end_tile.x, end_tile.y);

    // Start the path at the end tile.
    path.push_front({current_tile.first, current_tile.second});

    // Until the current tile of the path is the same as the start_tile, add
    // it to the path deque and set the tile to the next tile in the path.
    while (came_from[current_tile] != std::make_pair(start_tile.x, start_tile.y))
    {
        current_tile = came_from[current_tile];
        path.push_front({current_tile.first, current_tile.second});
    }

    // End the path at the start tile.
    path.push_front(start_tile);

    return path;
}

void Level::checkTile(std::queue<SDL_Point>& frontier, const int x, const int y, const int orig_x, const int orig_y)
{
    // If the tile is traversable.
    if (grid[y][x] == '1')
    {
        // If it has not been visited before.
        if (came_from.find(std::make_pair(x, y)) == came_from.end())
        {
            // Add it to the frontier.
            frontier.push({x, y});

            // And store it in the path.
            came_from[std::make_pair(x, y)] = std::make_pair(orig_x, orig_y);
        }
    }
}
