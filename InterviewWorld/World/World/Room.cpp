#include "room.h"
#include "functors.h"
#include "character.h"
#include "script_engine.h"
#include "world.h"

std::unordered_map<std::string, Room> Room::AllRooms;


Room::Room(const std::string& roomID) {
    if (AllRooms.find(roomID) == AllRooms.end()) {
        throw std::runtime_error("Room with ID '" + roomID + "' not found.");
    }
    *this = AllRooms[roomID];
}

// Default constructor
Room::Room() : RoomKeyID(""), Description(""), CanEnterCondition("true") {}

void Room::ConsumeRoomJson(const std::vector<std::string>& jsonFilePaths) {
    AllRooms.clear();

    for (const auto& filePath : jsonFilePaths) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Warning: Failed to open JSON file: " << filePath << "\n";
            continue;
        }

        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        for (const auto& room : jsonData) {
            Room newRoom;
            newRoom.RoomKeyID = room["RoomKeyID"];
            newRoom.Description = room.value("Description", "No description available");
            newRoom.VectorOfActions = room.value("Actions", std::vector<std::string>());
            newRoom.VectorOfRoomID = room.value("ConnectedRooms", std::vector<std::string>());
            newRoom.RoomState = room.value("State", std::unordered_map<std::string, std::string>());

            //  Store the script-based condition as a string (instead of functor)
            newRoom.CanEnterCondition = room.value("CanEnter", "true"); // Default to always true

            AllRooms[newRoom.RoomKeyID] = newRoom;
        }
    }
}

//  Modify CanEnter() to evaluate script condition
bool Room::CanEnter() const {
    return ScriptEngine::EvaluateCondition(CanEnterCondition, &World::Instance->CurrentRoom, nullptr);
}

bool Room::CanMoveTo(const std::string& roomID) const {
    if (AllRooms.find(roomID) == AllRooms.end()) return false;
    return AllRooms.at(roomID).CanEnter() ? AllRooms.at(roomID).CanEnter() : true;
}


bool Room::MoveTo(const std::string& roomID) {
    if (CanMoveTo(roomID)) {
        *this = AllRooms.at(roomID);
        PrintRoomDetails();
        return true;
    }
    std::cout << "You can't move to that room.\n";
    return false;
}


void Room::ListActions() const {
    std::cout << "Available Actions:\n";
    for (const auto& actionID : VectorOfActions) {
        if (Action::actionsDictionary.find(actionID) != Action::actionsDictionary.end()) {
            const Action& action = Action::actionsDictionary.at(actionID);
            auto canDoResult = action.CanDoAction();
            if (canDoResult.first) {
                std::cout << "  ACTION: " << canDoResult.second << " KEY :" <<action.id << "\n";
            }
        }
    }
}


void Room::ListConnectedRooms() const {
    std::cout << "Connected Rooms:\n";
    for (const auto& roomID : VectorOfRoomID) {
        if (AllRooms.find(roomID) != AllRooms.end() && CanMoveTo(roomID)) {
            std::cout << "  ROOM: " << AllRooms.at(roomID).Description << " KEY:" << AllRooms.at(roomID).RoomKeyID << "\n";
        }
    }
}


void Room::PrintRoomState() const {
    std::cout << "Room State Variables:\n";
    for (const auto& [key, value] : RoomState) {
        std::cout << "  " << key << ": " << value << "\n";
    }
}


void Room::SetRoomState(const std::string& key, const std::string& value) {
    RoomState[key] = value;
}

std::string Room::GetRoomState(const std::string& key) const {
    auto it = RoomState.find(key);
    return (it != RoomState.end()) ? it->second : "N/A";
}


void Room::PrintRoomDetails() const {
    std::cout << "---------------------------------\n";
    std::cout << "You are in: " << Description << "\n";

    //  Print room state variables
    std::cout << "Room State Variables:\n";
    for (const auto& [key, value] : RoomState) {
        std::cout << "  " << key << ": " << value << "\n";
    }

    //  Print connected rooms
    std::cout << "Connected Rooms:\n";
    for (const auto& roomID : VectorOfRoomID) {
        if (AllRooms.find(roomID) != AllRooms.end() && CanMoveTo(roomID)) {
            std::cout << "  ROOM: " << AllRooms.at(roomID).Description << " (KEY: " << roomID << ")\n";
        }
    }

    //  Print available actions
    std::cout << "Available Actions:\n";
    for (const auto& actionID : VectorOfActions) {
        if (Action::actionsDictionary.find(actionID) != Action::actionsDictionary.end()) {
            const Action& action = Action::actionsDictionary.at(actionID);
            auto canDoResult = action.CanDoAction();
            if (canDoResult.first) {
                std::cout << "  ACTION: " << canDoResult.second << " (KEY: " << actionID << ")\n";
            }
        }
    }

    //  Print characters in the room
    std::cout << "Characters Present:\n";
    for (const auto& [charID, character] : Character::AllCharacters) {
        if (character.GetCurrentRoomID() == RoomKeyID) {
            std::cout << "  Description: " << character.GetDescription() << " : KEY: " << charID << "\n";
        }
    }

    std::cout << "---------------------------------\n";
}

