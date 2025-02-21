#include "world.h"


World* World::Instance = nullptr; 

World::World(const std::string& startRoomID) : CurrentRoom(startRoomID) {
    Instance = this;
}
void World::MoveToRoom(const std::string& roomID) {
    if (CurrentRoom.MoveTo(roomID)) {
        CurrentRoom = Room(roomID);
    }
}

void World::ExecuteAction(const std::string& actionID) {
    if (Action::actionsDictionary.find(actionID) != Action::actionsDictionary.end()) {
        auto result = Action::actionsDictionary.at(actionID).DoAction();
        std::cout << result.second << "\n";
    }
    else {
        std::cout << "Invalid action.\n";
    }
}
