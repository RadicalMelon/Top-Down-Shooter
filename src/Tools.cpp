#include "Tools.h"

/**
* This namespace contains helpful functions that can be used anywhere in
* the program.
*/
namespace Tools
{
    /**
    * This function splits a string into a vector of strings. The string is split
    * based on the delimeter.
    */
    std::vector<std::string> splitString(const std::string& text, const char delimeter)
    {
        std::vector<std::string> split;
        std::stringstream ss(text);
        std::string item;

        while (std::getline(ss, item, delimeter))
        {
            split.push_back(item);
        }

        return split;
    }

    /**
    * This function calculates the angle between two points.
    */
    int angleBetweenPoints(const int x1, const int y1, const int x2, const int y2)
    {
        return ((std::atan2(y1 - y2, x1 - x2) * 180.0000) / 3.1416);
    }

    /**
    * This function returns a normalized vector.
    */
    FloatVector normalizeVector(const SDL_Point& vec)
    {
        float length = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
        return {static_cast<float>(vec.x) / length, static_cast<float>(vec.y) / length};
    }

    FloatVector normalizeVector(const FloatVector& vec)
    {
        float length = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
        return {vec.x / length, vec.y / length};
    }

    /**
    * This function returns a random number from a range.
    */
    int getRandomInt(const int from, const int to)
    {
        static std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> dist(from, to);
        return dist(engine);
    }

    float getRandomFloat(const float from, const float to)
    {
        static std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(from, to);
        return dist(engine);
    }
}
