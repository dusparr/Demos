#ifndef WORLD_H
#define WORLD_H

#include "room.h"

class World {
public:
    static World* Instance; 
    Room CurrentRoom;
    std::unordered_map<std::string, std::string> WorldVariables;

    World(const std::string& startRoomID);

    void MoveToRoom(const std::string& roomID);
    void ExecuteAction(const std::string& actionID);
};

#endif // WORLD_H
