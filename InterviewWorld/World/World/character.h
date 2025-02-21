#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include "room.h"

class Character {
public:
    std::string CharacterID;
    std::string Name;
    std::string Description;
    std::unordered_map<std::string, std::string> CharacterState;
    std::string CurrentRoomID;
    std::function<bool()> CanMove;  // Function to determine if the character can move
    std::string CanMoveCondition;  //  Store movement condition script

    bool CanMoveToRoom() const;

    // Static dictionary for all characters
    static std::unordered_map<std::string, Character> AllCharacters;
    std::string GetCurrentRoomID() const { return CurrentRoomID; }
    std::string GetDescription() const { return Description; }
    std::string GetPrompt() const;


public:
    // Default constructor
    Character();

    // Constructor that initializes a character from the dictionary
    Character(const std::string& characterID);

    // Static method to load characters from JSON
    static void ConsumeCharacterJson(const std::vector<std::string>& jsonFilePaths);

    // Move to another room
    bool MoveToRoom(const std::string& roomID);

    // Modify character state
    void SetCharacterState(const std::string& key, const std::string& value);
    std::string GetCharacterState(const std::string& key) const;

    // Print character details
    void PrintCharacterDetails() const;
};

#endif // CHARACTER_H
