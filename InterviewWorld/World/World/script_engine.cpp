

#include <string>
#include <unordered_map>
#include <iostream>
#include "room.h"
#include "character.h"
#include "script_engine.h"

    

    


    //  Trim function to remove spaces
    static std::string Trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }
    //  Executes an action like "room.door_state = 'unlocked'"
    static void ExecuteScript(std::string script, Room* currentRoom, Character* currentCharacter) {
        if (script.empty()) return;

        size_t pos = script.find("=");
        if (pos == std::string::npos) return;  // Not an assignment

        std::string varName = Trim(script.substr(0, pos));
        std::string value = Trim(script.substr(pos + 1));

        //  Remove quotes from value
        if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.size() - 2);
        }

        //  Check where the assignment goes
        if (varName.find("room.") == 0 && currentRoom) {
            std::string key = varName.substr(5);  // Remove "room."
            currentRoom->SetRoomState(key, value);
        }
        else if (varName.find("character.") == 0 && currentCharacter) {
            std::string key = varName.substr(9);  // Remove "character."
            currentCharacter->SetCharacterState(key, value);
        }
        else {
            //  Assign to specific rooms or characters
            size_t dotPos = varName.find(".");
            if (dotPos != std::string::npos) {
                std::string id = varName.substr(0, dotPos);
                std::string key = varName.substr(dotPos + 1);

                if (Room::AllRooms.find(id) != Room::AllRooms.end()) {
                    Room::AllRooms[id].SetRoomState(key, value);
                }
                else if (Character::AllCharacters.find(id) != Character::AllCharacters.end()) {
                    Character::AllCharacters[id].SetCharacterState(key, value);
                }
            }
        }
    }
    //  Splits a string into two parts based on a delimiter
    static std::vector<std::string> Split(const std::string& str, const std::string& delimiter) {
        size_t pos = str.find(delimiter);
        if (pos == std::string::npos) return { str };

        return { str.substr(0, pos), str.substr(pos + delimiter.length()) };
    }
    //  Simple parser for logical expressions (supports &&, ||, ==, !=)
    static bool ParseLogicalExpression(const std::string& expr) {
        if (expr.find("&&") != std::string::npos) {
            auto parts = Split(expr, "&&");
            return ParseLogicalExpression(parts[0]) && ParseLogicalExpression(parts[1]);
        }
        if (expr.find("||") != std::string::npos) {
            auto parts = Split(expr, "||");
            return ParseLogicalExpression(parts[0]) || ParseLogicalExpression(parts[1]);
        }
        if (expr.find("==") != std::string::npos) {
            auto parts = Split(expr, "==");
            return Trim(parts[0]) == Trim(parts[1]);
        }
        if (expr.find("!=") != std::string::npos) {
            auto parts = Split(expr, "!=");
            return Trim(parts[0]) != Trim(parts[1]);
        }
        return false;
    }

    
    bool ScriptEngine::EvaluateCondition(std::string script, const Room* currentRoom, const Character* currentCharacter) {
        if (script.empty()) return false;

        std::unordered_map<std::string, std::string> variableValues;

        //  Inject Current Room Variables
        if (currentRoom) {
            for (const auto& pair : currentRoom->RoomState) {
                variableValues["room." + pair.first] = pair.second;
            }
        }

        //  Inject Current Character Variables
        if (currentCharacter) {
            for (const auto& pair : currentCharacter->CharacterState) {
                variableValues["character." + pair.first] = pair.second;
            }
        }

        //  Inject All Room Variables
        for (const auto& pair : Room::AllRooms) {
            const Room& room = pair.second;
            for (const auto& state : room.RoomState) {
                variableValues[room.RoomKeyID + "." + state.first] = state.second;
            }
        }

        //  Inject All Character Variables
        for (const auto& pair : Character::AllCharacters) {
            const Character& character = pair.second;
            for (const auto& state : character.CharacterState) {
                variableValues[character.CharacterID + "." + state.first] = state.second;
            }
        }

        //  Replace Variables in Script
        for (const auto& var : variableValues) {
            std::string placeholder = var.first;
            size_t pos = script.find(placeholder);
            if (pos != std::string::npos) {
                script.replace(pos, placeholder.length(), "\"" + var.second + "\"");
            }
        }

        //  Manually parse the condition
        return ParseLogicalExpression(script);
    }
