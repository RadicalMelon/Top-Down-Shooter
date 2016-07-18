#include "Application.h"

/**
* This namespace is used to contain all of the important game information. It is
* responsible for handling resources, game states, and the main game loop.
*/
namespace Application
{
    /**
    * This anonymous namespace holds all of the objects and variables for the
    * game. It holds the window, renderer, resource maps, states and more. Functions
    * are used to access some of these whenever they are neeeded.
    */
    namespace
    {
        // SDL objects.
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Event event;
        SDL_Point window_size;
        SDL_Point camera;
        SDL_Point mouse_position;

        // Game states.
        StateMap states;
        std::string current_state_name;
        std::shared_ptr<BaseState> current_state;

        // Resource maps.
        std::map<std::string, SDL_Texture*> textures;
        std::map<std::string, Mix_Chunk*> sounds;
        std::map<std::string, std::map<int, TTF_Font*>> fonts;

        // Config variables
        std::map<std::string, std::map<std::string, std::string>> config;
        std::string config_file_name;
        std::vector<std::string> catagory_order;
        std::vector<std::string> setting_order;

        // Miscellaneous variables.
        bool running;
        float delta_time;
        int frame_rate_limit;
        int frame_rate;
        std::string window_title;
    }

    /**
    * Load an XML config file for use in the game. The config object is a map
    * of maps, so individual settings have to be grouped together otherwise an error will occur.
    */
    void loadConfig(const std::string& file_name)
    {
        config_file_name = file_name;

        // Open the config file.
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(config_file_name.c_str()) != tinyxml2::XML_SUCCESS)
        {
            SDL_SetError(doc.ErrorName());
            throw Error::XML;
        }

        // Load every setting into a map.
        for (tinyxml2::XMLNode* catagory_node = doc.FirstChild(); catagory_node; catagory_node = catagory_node->NextSibling())
        {
            std::string catagory_name = catagory_node->ToElement()->Name();

            // Remember the order of the catagories.
            catagory_order.push_back(catagory_name);

            for (tinyxml2::XMLNode* setting_node = catagory_node->FirstChild(); setting_node; setting_node = setting_node->NextSibling())
            {
                std::string setting_name = setting_node->ToElement()->Name();

                // Remember the order of the individual settings.
                setting_order.push_back(setting_name);

                std::string setting = setting_node->ToElement()->GetText();
                config[catagory_name][setting_name] = setting;
            }
        }
    }

    /**
    * This function takes the config map and writes it the same file it was loaded
    * from. It also writes it in the same order, to avoid confusion when editing the config
    * file.
    */
    void writeConfig()
    {
        tinyxml2::XMLDocument doc;

        tinyxml2::XMLElement* catagory_element;
        tinyxml2::XMLElement* setting_element;

        for (unsigned i = 0; i < catagory_order.size(); i++)
        {
            catagory_element = doc.NewElement(catagory_order[i].c_str());
            for (unsigned j = 0; j < setting_order.size(); j++)
            {
                if (config[catagory_order[i]].find(setting_order[j]) != config[catagory_order[i]].end())
                {
                    setting_element = doc.NewElement(setting_order[j].c_str());
                    setting_element->SetText(config[catagory_order[i]][setting_order[j]].c_str());
                    catagory_element->LinkEndChild(setting_element);
                }
            }
            doc.LinkEndChild(catagory_element);
        }

        doc.SaveFile(config_file_name.c_str());
    }

    /**
    * This function initializes SDL and its extensions, it also creates a window and
    * renderer. This should be the first function in this namespace that is called.
    */
    void startUp(const std::string& title, const int width, const int height, const int fps_limit, const bool fullscreen)
    {
        // Initialize SDL.
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
        {
            throw Error::SDL;
        }

        // Initialize SDL_image.
        if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
        {
            throw Error::IMG;
        }

        // Initialize SDL_mixer.
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
        {
            throw Error::Mix;
        }

        // Initialize SDL_ttf.
        if (TTF_Init() == -1)
        {
            throw Error::TTF;
        }

        // Create a window.
        window_title = title;
        window_size.x = width;
        window_size.y = height;
        if (fullscreen)
        {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, SDL_WINDOW_FULLSCREEN);
        }
        else
        {
            window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_size.x, window_size.y, SDL_WINDOW_SHOWN);
        }
        if (window == nullptr)
        {
            throw Error::SDL;
        }

        // Create a renderer.
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr)
        {
            throw Error::SDL;
        }

        running = true;
        frame_rate_limit = fps_limit;
        frame_rate = 0;
    }

    /**
    * This function takes a StateMap and a string representing the state the game should
    * start on. States should be created and this should be called after the startUp function
    * is called.
    */
    void setupStates(const StateMap& state_map, const std::string& state_name)
    {
        states = state_map;
        current_state_name = state_name;
        current_state = states[current_state_name];
    }

    /**
    * This function changes the current state of the game. It calls the shutDown method
    * of the current state and the startUp method of the new state.
    */
    void changeState(const std::string& state_name)
    {
        current_state->shutDown();
        current_state_name = state_name;
        current_state = states[current_state_name];
        current_state->startUp();
    }

    /**
    * This function runs the main game loop.
    */
    void run()
    {
        int last_time = 0;
        int current_time = 0;
        int frame_time = 0;

        current_state->startUp();
        while (running)
        {
            // Work out the time since the last frame.
            last_time = current_time;
            current_time = SDL_GetTicks();
            frame_time = current_time - last_time;

            // Limit the framerate based on the time since the last frame.
            if ((1000.0 / frame_rate_limit) > frame_time)
            {
                SDL_Delay((1000.0 / frame_rate_limit) - frame_time);
            }

            // Calculate the current frame rate.
            frame_rate = 1000.0 / frame_time;

            // Calculate the current delta time.
            delta_time = frame_time / 1000.0;

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }
                else if (event.type == SDL_MOUSEMOTION)
                {
                    // Keep track of the position of the mouse.
                    mouse_position.x = event.motion.x;
                    mouse_position.y = event.motion.y;
                }
                current_state->handleEvents();
            }

            current_state->update();

            SDL_RenderClear(renderer);
            current_state->draw();
            SDL_RenderPresent(renderer);
        }
        current_state->shutDown();
    }

    /**
    * This function destroys the window, the renderer and all of the resources loaded in the game.
    * It also shuts down SDL and its extensions.
    */
    void shutDown()
    {
        for (auto& texture : textures)
        {
            DEBUG("Unloading texture: " << texture.first);
            SDL_DestroyTexture(texture.second);
        }

        for (auto& sound : sounds)
        {
            DEBUG("Unloading sound: " << sound.first);
            Mix_FreeChunk(sound.second);
        }

        for (auto& font : fonts)
        {
            for (auto& font_it : font.second)
            {
                DEBUG("Unloading font, size " << font_it.first << ": " << font.first);
                TTF_CloseFont(font_it.second);
            }
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
    }

    /**
    * This function loads a texture and then caches it, so that the next time the texture is
    * needed it is available immediately.
    */
    SDL_Texture* getTexture(const std::string& file_name)
    {
        if (textures.find(file_name) == textures.end())
        {
            DEBUG("Loading texture: " << file_name);

            textures[file_name] = IMG_LoadTexture(renderer, file_name.c_str());
            if (textures[file_name] == nullptr)
            {
                throw Error::IMG;
            }
        }
        return textures[file_name];
    }

    /**
    * This function loads a sound and then caches it, so that the next time the sound is
    * needed it is available immediately.
    */
    Mix_Chunk* getSound(const std::string& file_name)
    {
        if (sounds.find(file_name) == sounds.end())
        {
            DEBUG("Loading sound: " << file_name);

            sounds[file_name] = Mix_LoadWAV(file_name.c_str());
            if (sounds[file_name] == nullptr)
            {
                throw Error::Mix;
            }
        }
        return sounds[file_name];
    }

    /**
    * This function loads a font and then caches it, so that the next time the font is
    * needed it is available immediately. Because SDL_ttf needs a font to be loaded with a size,
    * a font has to be identified by its name and size.
    */
    TTF_Font* getFont(const std::string& file_name, const int font_size)
    {
        if (fonts.find(file_name) == fonts.end() || fonts.find(file_name)->second.find(font_size) == fonts.find(file_name)->second.end())
        {
            DEBUG("Loading font, size " << font_size << ": " << file_name);

            fonts[file_name][font_size] = TTF_OpenFont(file_name.c_str(), font_size);
            if (fonts[file_name][font_size] == nullptr)
            {
                throw Error::TTF;
            }
        }
        return fonts[file_name][font_size];
    }

    /**
    * This function returns the game window.
    */
    SDL_Window* getWindow()
    {
        return window;
    }

    /**
    * This function returns the game renderer.
    */
    SDL_Renderer* getRenderer()
    {
        return renderer;
    }

    /**
    * This function returns to the game's event.
    */
    const SDL_Event& getEvent()
    {
        return event;
    }

    /**
    * This function returns an SDL_Point of the window's size.
    */
    const SDL_Point& getWindowSize()
    {
        return window_size;
    }

    /**
    * This function returns the current position of the mouse.
    */
    const SDL_Point& getMousePosition()
    {
        return mouse_position;
    }

    /**
    * This function returns the game camera for drawing and editing.
    */
    SDL_Point& getCamera()
    {
        return camera;
    }

    /**
    * This function converts a rect so that it is consistent with
    * the camera's view.
    */
    SDL_Rect convertToCameraView(const SDL_Rect& rect)
    {
        return {rect.x + camera.x, rect.y + camera.y, rect.w, rect.h};
    }

    /**
    * This function returns the config map, which contains settings for the game.
    */
    std::map<std::string, std::map<std::string, std::string>>& getConfigMap()
    {
        return config;
    }

    /**
    * This function returns the time, in seconds, since the last frame.
    */
    float getDeltaTime()
    {
        return delta_time;
    }

    /**
    * This function ends the game by stopping the main loop.
    */
    void quit()
    {
        running = false;
    }

    /**
    * This function returns the current title of the window.
    */
    std::string getWindowTitle()
    {
        return window_title;
    }

    /**
    * This function sets the title of the window.
    */
    void setWindowTitle(const std::string& title)
    {
        window_title = title;
        SDL_SetWindowTitle(window, window_title.c_str());
    }

    /**
    * This function gets the current frame rate for the game.
    */
    int getFrameRate()
    {
        return frame_rate;
    }
}
