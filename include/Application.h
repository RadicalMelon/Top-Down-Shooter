#ifndef APPLICATION_H
#define APPLICATION_H

#include "Debug.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <tinyxml2.h>

/**
* This namespace is used to contain all of the important game information. It is
* responsible for handling resources, game states, and the main game loop.
*/
namespace Application
{
    /**
    * This enum contains all of the errors that can be thrown within this namespace,
    * these errors must be handled outside of the namespace.
    */
    enum class Error
    {
        SDL,
        IMG,
        TTF,
        Mix,
        XML
    };

    /**
    * This is a pure virtual class that should be used for every state in the game.
    * Every state class must inherit from this for it to be accepted into the namespace.
    */
    class BaseState
    {
    public:
        virtual void startUp() = 0;
        virtual void handleEvents() = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
        virtual void shutDown() = 0;
    };

    /**
    * This typedef is a map of every state that can be in the game.
    */
    typedef std::map<std::string, std::shared_ptr<BaseState>> StateMap;

    /**
    * This anonymous namespace holds all of the objects and variables for the
    * game. It holds the window, renderer, resource maps, states and more. Functions
    * are used to access some of these whenever they are neeeded.
    */
    namespace
    {
        // SDL objects.
        extern SDL_Window* window;
        extern SDL_Renderer* renderer;
        extern SDL_Event event;
        extern SDL_Point window_size;
        extern SDL_Point camera;
        extern SDL_Point mouse_position;

        // Game states.
        extern StateMap states;
        extern std::string current_state_name;
        extern std::shared_ptr<BaseState> current_state;

        // Resource maps.
        extern std::map<std::string, SDL_Texture*> textures;
        extern std::map<std::string, Mix_Chunk*> sounds;
        extern std::map<std::string, std::map<int, TTF_Font*>> fonts;

        // Config variables.
        extern std::map<std::string, std::map<std::string, std::string>> config;
        extern std::string config_file_name;
        extern std::vector<std::string> catagory_order;
        extern std::vector<std::string> setting_order;

        // Miscellaneous variables.
        extern bool running;
        extern float delta_time;
        extern int frame_rate_limit;
        extern int frame_rate;
        extern std::string window_title;
    }

    /**
    * This function loads an XML config file for use in the game. The config object is a map
    * of maps, so individual settings have to be grouped together otherwise an error will occur.
    */
    void loadConfig(const std::string& file_name);

    /**
    * This function takes the config map and writes it the same file it was loaded
    * from.
    */
    void writeConfig();

    /**
    * This function initializes SDL and its extensions, it also creates a window and
    * renderer. This should be the first function in this namespace that is called.
    */
    void startUp(const std::string& title, const int width, const int height, const int fps_limit, const bool fullscreen);

    /**
    * This function takes a StateMap and a string representing the state the game should
    * start on. States should be created and this should be called after the startUp function
    * is called.
    */
    void setupStates(const StateMap& states, const std::string& state_name);

    /**
    * This function changes the current state of the game. It calls the shutDown method
    * of the current state and the startUp method of the new state.
    */
    void changeState(const std::string& state_name);

    /**
    * This function runs the main game loop.
    */
    void run();

    /**
    * This function destroys the window, the renderer and all of the resources loaded in the game.
    * It also shuts down SDL and its extensions.
    */
    void shutDown();

    /**
    * This function loads a texture and then caches it, so that the next time the texture is
    * needed it is available immediately.
    */
    SDL_Texture* getTexture(const std::string& file_name);

    /**
    * This function loads a sound and then caches it, so that the next time the sound is
    * needed it is available immediately.
    */
    Mix_Chunk* getSound(const std::string& file_name);

    /**
    * This function loads a font and then caches it, so that the next time the font is
    * needed it is available immediately. Because SDL_ttf needs a font to be loaded with a size,
    * a font has to be identified by its name and size.
    */
    TTF_Font* getFont(const std::string& file_name, const int font_size);

    /**
    * This function returns the game window.
    */
    SDL_Window* getWindow();

    /**
    * This function returns the game renderer.
    */
    SDL_Renderer* getRenderer();

    /**
    * This function returns to the game's event.
    */
    const SDL_Event& getEvent();

    /**
    * This function returns an SDL_Point of the window's size.
    */
    const SDL_Point& getWindowSize();

    /**
    * This function returns the current position of the mouse.
    */
    const SDL_Point& getMousePosition();

    /**
    * This function returns the game camera for drawing and editing.
    */
    SDL_Point& getCamera();

    /**
    * This function converts a rect so that it is consistent with
    * the camera's view.
    */
    SDL_Rect convertToCameraView(const SDL_Rect& rect);

    /**
    * This function returns the config map, which contains settings for the game.
    */
    std::map<std::string, std::map<std::string, std::string>>& getConfigMap();

    /**
    * This function returns the time, in seconds, since the last frame.
    */
    float getDeltaTime();

    /**
    * This function ends the game by stopping the main loop.
    */
    void quit();

    /**
    * This function returns the current title of the window.
    */
    std::string getWindowTitle();

    /**
    * This function sets the title of the window.
    */
    void setWindowTitle(const std::string& title);

    /**
    * This function gets the current frame rate for the game.
    */
    int getFrameRate();
}

#endif // APPLICATION_H
