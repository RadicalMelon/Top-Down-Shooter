#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

// Comment out if no debug is wanted.
#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
    #define DEBUG(out) std::cout << out << std::endl;
#else
    #define DEBUG(out)
#endif // DEBUG_ENABLED

#endif // DEBUG_H
