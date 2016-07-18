#include "Debug.h"
#include "GameState.h"

int main(int argc, char* argv[])
{
    try
    {
        Application::loadConfig("Resources/Config.xml");
        int width = std::atoi(Application::getConfigMap()["screen"]["width"].c_str());
        int height = std::atoi(Application::getConfigMap()["screen"]["height"].c_str());
        int fps = std::atoi(Application::getConfigMap()["screen"]["fps_limit"].c_str());

        bool fullscreen = false;
        if (Application::getConfigMap()["screen"]["fullscreen"] == "true")
        {
            fullscreen = true;
        }

        Application::startUp("Top Down Shooter", width, height, fps, fullscreen);
        Application::StateMap states = {
            {"GAME", std::make_shared<GameState>()}
        };
        Application::setupStates(states, "GAME");
        Application::run();

        Application::shutDown();
    }
    catch (const Application::Error& error)
    {
        switch (error)
        {
        case Application::Error::SDL:
            DEBUG("SDL_Error: " << SDL_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", SDL_GetError(), Application::getWindow());
            break;
        case Application::Error::IMG:
            DEBUG("IMG Error: " << IMG_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "IMG Error", IMG_GetError(), Application::getWindow());
            break;
        case Application::Error::TTF:
            DEBUG("TTF Error: " << TTF_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TTF Error", TTF_GetError(), Application::getWindow());
            break;
        case Application::Error::Mix:
            DEBUG("Mix Error: " << Mix_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Mix Error", Mix_GetError(), Application::getWindow());
            break;
        case Application::Error::XML:
            DEBUG("XML Error: " << SDL_GetError());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "XML Error", SDL_GetError(), Application::getWindow());
            break;
        default:
            break;
        }

        Application::shutDown();

#ifdef DEBUG_ENABLED
        std::cin.get();
#endif // DEBUG_ENABLED
    }

    return 0;
}
