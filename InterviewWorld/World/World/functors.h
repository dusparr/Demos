#ifndef FUNCTORS_H
#define FUNCTORS_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include "world.h"

class Functors {
public:
    static std::unordered_map<std::string, std::function<bool()>> functorRegistry;

    static void RegisterFunctors();

    static bool SwitchLight();
    static bool CheckLightBlack();
    static bool CheckLightWhite();
};

#endif // FUNCTORS_H
