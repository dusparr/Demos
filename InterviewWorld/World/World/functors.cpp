#include "functors.h"


std::unordered_map<std::string, std::function<bool()>> Functors::functorRegistry;
bool LightTorch() {
    if (!World::Instance) return false; // Ensure World is available

    std::string currentLight = World::Instance->CurrentRoom.GetRoomState("TorchLit");
    if (currentLight == "False")
    {
        std::cout << "You Light the torch, revealing the passage ahead. \n";
        World::Instance->CurrentRoom.SetRoomState("TorchLit", "True");
        return true;
    }
    else if (currentLight == "True")
    {
        std::cout << "The Torch is already lit, no need to relight it. \n";
        return false;
    }
    else
    {
        std::cout << "There is no torch to light in this room. \n";
        return false;
    }
    
}
bool takeKey() {
    if (!World::Instance) return false; // Ensure World is available

    std::string DungeonKey = World::Instance->CurrentRoom.GetRoomState("DungeonKey");
    if (DungeonKey == "true")
    {
        std::cout << "You take the key and poket it. \n";
        World::Instance->CurrentRoom.SetRoomState("DungeonKey", "false");
        World::Instance->WorldVariables["HaveDungeonKey1"] = "true";
        return true;
    }
    else if (DungeonKey == "True")
    {
        std::cout << "nothing to take. \n";
        return false;
    }
    else
    {
        std::cout << ".... \n";
        return false;
    }

}
bool search_wall() {
    if (!World::Instance) return false; // Ensure World is available

    std::string SecretDoor = World::Instance->CurrentRoom.GetRoomState("SecretDoor");
    if (SecretDoor == "False")
    {
        std::cout << "you search the wall, finding a secret passage. \n";
        World::Instance->CurrentRoom.SetRoomState("SecretDoor", "True");
        return true;
    }
    else if (SecretDoor == "True")
    {
        std::cout << "no need to re-search the room. \n";
        return false;
    }
    else
    {
        std::cout << ".... \n";
        return false;
    }

}
bool CheckTorch() {
    if (!World::Instance) return false; // Ensure World is available

    std::string currentLight = World::Instance->CurrentRoom.GetRoomState("TorchLit");
    if (currentLight == "False")
    {
        return true;
    }
    else if (currentLight == "True")
    {
        return false;
    }
    else
    {
        return false;
    }

}

void Functors::RegisterFunctors() {
    functorRegistry["always_success"] = []() -> bool { return true; };
    functorRegistry["always_fail"] = []() -> bool { return false; };

    functorRegistry["switch_light"] = []() -> bool {
        return Functors::SwitchLight();
        };

    functorRegistry["check_light_black"] = []() -> bool {
        return Functors::CheckLightBlack();
        };

    functorRegistry["check_light_white"] = []() -> bool {
        return Functors::CheckLightWhite();
        };
    
    functorRegistry["light_torch"] = []() -> bool { return LightTorch(); };
    functorRegistry["search_wall"] = []() -> bool { return search_wall(); };
    functorRegistry["take_key"] = []() -> bool { return takeKey(); };
    




}

bool Functors::SwitchLight() {
    if (!World::Instance) return false; // Ensure World is available

    std::string currentLight = World::Instance->CurrentRoom.GetRoomState("Light");
    std::string newLight = (currentLight == "Black") ? "White" : "Black";
    World::Instance->CurrentRoom.SetRoomState("Light", newLight);
    std::cout << "The light changes to " << newLight << ".\n";
    return true;
}



bool Functors::CheckLightBlack() {
    if (!World::Instance) return false;
    return (World::Instance->CurrentRoom.GetRoomState("Light") == "Black") ;
}


bool Functors::CheckLightWhite() {
    if (!World::Instance) return false;
    return (World::Instance->CurrentRoom.GetRoomState("Light") == "White") ;
}
