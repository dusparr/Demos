#ifndef ROOM_H
#define ROOM_H

#include "action.h"

class Room {
public:
    std::string RoomKeyID;
    std::string Description;
    std::vector<std::string> VectorOfActions;
    std::vector<std::string> VectorOfRoomID;
    std::unordered_map<std::string, std::string> RoomState; //  Room key-value dictionary
    std::string CanEnterCondition;

    // Static dictionary for all rooms
    static std::unordered_map<std::string, Room> AllRooms;

public:

    // Default constructor
    Room();

    // Constructor that initializes a room from the dictionary
    Room(const std::string& roomID);

    // Static method to load rooms from JSON
    static void ConsumeRoomJson(const std::vector<std::string>& jsonFilePaths);

    bool CanMoveTo(const std::string& roomID) const;
    bool MoveTo(const std::string& roomID);
    void ListActions() const;
    void ListConnectedRooms() const;
    bool CanEnter() const;
    void PrintRoomDetails() const;

    //  Print the Room's Key-Value States
    void PrintRoomState() const;

    //  Modify Room State
    void SetRoomState(const std::string& key, const std::string& value);
    std::string GetRoomState(const std::string& key) const;
};

#endif // ROOM_H
