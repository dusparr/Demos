#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <iostream>
#include <string>
#include <unordered_map>

class ScriptEngine {
public:
    //  Evaluates a condition stored as a string
    static bool EvaluateCondition(std::string script,const Room* currentRoom, const Character* currentCharacter);
};

#endif // SCRIPT_ENGINE_H
