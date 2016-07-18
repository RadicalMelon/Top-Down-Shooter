#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

#include <SDL.h>

/**
* This namespace contains helpful functions that can be used anywhere in
* the program.
*/
namespace Tools
{
    /**
    * This is a type used for normalizing vectors, since
    * normalized vectors are always floats.
    */
    struct FloatVector
    {
        float x, y;
    };

    /**
    * This function splits a string into a vector of strings. The string is split
    * based on the delimeter.
    */
    std::vector<std::string> splitString(const std::string& text, const char delimeter);

    /**
    * This function calculates the angle between two points.
    */
    int angleBetweenPoints(const int x1, const int y1, const int x2, const int y2);

    /**
    * This function returns a normalized vector.
    */
    FloatVector normalizeVector(const SDL_Point& vec);
    FloatVector normalizeVector(const FloatVector& vec);

    /**
    * This function returns a random number from a range.
    */
    int getRandomInt(const int from, const int to);

    float getRandomFloat(const float from, const float to);
}

#endif // TOOLS_H
