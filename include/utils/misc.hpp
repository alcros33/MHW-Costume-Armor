#include <iostream>

#ifndef NDEBUG
    #define DEBUG_LOG(X) std::cout<< X <<std::endl
    #define DEBUG_LOG_HEX(X) std::cout << std::hex << X << std::dec << std::endl
#else
    #define DEBUG_LOG(X)
    #define DEBUG_LOG_HEX(X)
#endif