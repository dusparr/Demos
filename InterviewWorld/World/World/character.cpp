#include "character.h"
#include "functors.h"
#include "script_engine.h"
#include <sstream>

//  Define the static character dictionary
std::unordered_map<std::string, Character> Character::AllCharacters;

//  Default constructor
Character::Character() : CharacterID(""), Name(""), Description(""), CurrentRoomID(""), CanMove(nullptr) {}

//  Constructor that initializes a character from the dictionary
Character::Character(const std::string& characterID) {
    if (AllCharacters.find(characterID) == AllCharacters.end()) {
        throw std::runtime_error("Character with ID '" + characterID + "' not found.");
    }
    *this = AllCharacters[characterID];
}

void Character::ConsumeCharacterJson(const std::vector<std::string>& jsonFilePaths) {
    AllCharacters.clear();

    for (const auto& filePath : jsonFilePaths) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Warning: Failed to open JSON file: " << filePath << "\n";
            continue;
        }

        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        for (const auto& character : jsonData) {
            Character newCharacter;
            newCharacter.CharacterID = character["CharacterID"];
            newCharacter.Name = character.value("Name", "Unknown Character");
            newCharacter.Description = character.value("Description", "No description available");
            newCharacter.CurrentRoomID = character.value("CurrentRoomID", "room_0_0");
            newCharacter.CharacterState = character.value("State", std::unordered_map<std::string, std::string>());

            //  Store movement condition as script
            newCharacter.CanMoveCondition = character.value("CanMove", "true");

            AllCharacters[newCharacter.CharacterID] = newCharacter;
        }
    }
}

//  Modify CanMoveToRoom() to evaluate script condition
bool Character::CanMoveToRoom() const {
    return ScriptEngine::EvaluateCondition(CanMoveCondition, &Room::AllRooms[this->CurrentRoomID], this);
}


//  Move to another room if allowed
bool Character::MoveToRoom(const std::string& roomID) {
    if (CanMoveToRoom()) {
        CurrentRoomID = roomID;
        std::cout << Name << " moved to " << CurrentRoomID << "\n";
        return true;
    }
    else {
        std::cout << Name << " cannot move to " << roomID << "\n";
        return false;
    }
}

//  Modify character state
void Character::SetCharacterState(const std::string& key, const std::string& value) {
    CharacterState[key] = value;
}


std::string Character::GetCharacterState(const std::string& key) const {
    auto it = CharacterState.find(key);
    return (it != CharacterState.end()) ? it->second : "N/A";
}

void Character::PrintCharacterDetails() const {
    std::cout << "---------------------------------\n";
    std::cout << "Character: " << Name << "\n";
    std::cout << Description << "\n";
    std::cout << "Current Room: " << CurrentRoomID << "\n";
    std::cout << "Character State:\n";
    for (const auto& [key, value] : CharacterState) {
        std::cout << "  " << key << ": " << value << "\n";
    }
    std::cout << "---------------------------------\n";
}

std::string Character::GetPrompt() const {
    std::ostringstream prompt;

    //  Character details
    prompt << "Character: " << Name << " - " << Description << "\n";

    //  Character states
    prompt << "Character States: ";
    if (CharacterState.empty()) {
        prompt << "None";
    }
    else {
        for (const auto& [key, value] : CharacterState) {
            prompt << key << ":" << value << ", ";
        }
        prompt.seekp(-2, std::ios_base::end); // Remove trailing ", "
    }
    prompt << "\n";

    //  Room details
    if (Room::AllRooms.find(CurrentRoomID) != Room::AllRooms.end()) {
        const Room& room = Room::AllRooms.at(CurrentRoomID);
        prompt << "Room: " << room.Description << "\n";

        //  Room states
        prompt << "Room States: ";
        if (room.RoomState.empty()) {
            prompt << "None";
        }
        else {
            for (const auto& [key, value] : room.RoomState) {
                prompt << key << ":" << value << ", ";
            }
            prompt.seekp(-2, std::ios_base::end); // Remove trailing ", "
        }
        prompt << "\n";
    }
    else {
        prompt << "Room: Unknown\n";
    }

    return prompt.str();
}