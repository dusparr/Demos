#include "action.h"
#include "functors.h"

// Initialize static dictionaries
std::unordered_map<std::string, Action> Action::actionsDictionary;


// Default constructor
Action::Action() : id(""), DescriptionOfAction(""), DescriptionOfTakingAction(""),
DescriptionOfTakingActionFail(""), CantDoAction(""),
Functor(nullptr), CanDoFunctor(nullptr) {
}
Action::Action(const std::string& actionID, const std::string& descAction,
    const std::string& descSuccess, const std::string& descFail, const std::string& cantDo)
    : id(actionID), DescriptionOfAction(descAction), DescriptionOfTakingAction(descSuccess),
    DescriptionOfTakingActionFail(descFail), CantDoAction(cantDo), Functor(nullptr) {
}

// Constructor: Load an action from the preloaded dictionary
Action::Action(const std::string& actionID) {
    if (actionsDictionary.find(actionID) == actionsDictionary.end()) {
        throw std::runtime_error("Action with ID '" + actionID + "' not found.");
    }
    *this = actionsDictionary[actionID];
}

// Static method to consume actions from JSON
void Action::ConsumeAllActions(const std::vector<std::string>& jsonFilePaths) {
    actionsDictionary.clear();
    for (const auto& filePath : jsonFilePaths) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open JSON file: " << filePath << "\n";
            continue;
        }
        nlohmann::json jsonData;
        file >> jsonData;
        file.close();

        for (const auto& action : jsonData) {
            std::string actionID = action["id"];
            std::string functorKey = action.value("Functor", "default");
            std::string canDoKey = action.value("CanDoFunctor", "default");

            Action newAction(actionID,
                action["DescriptionOfAction"],
                action["DescriptionOfTakingAction"],
                action["DescriptionOfTakingActionFail"],
                action["CantDoAction"]);

            if (Functors::functorRegistry.find(functorKey) != Functors::functorRegistry.end()) {
                newAction.Functor = Functors::functorRegistry[functorKey];
            }
            if (Functors::functorRegistry.find(canDoKey) != Functors::functorRegistry.end()) {
                newAction.CanDoFunctor = Functors::functorRegistry[canDoKey];
            }
            actionsDictionary[actionID] = newAction;
        }
    }
}

// Determines if action can be performed
std::pair<bool, std::string> Action::CanDoAction() const {
    return CanDoFunctor && CanDoFunctor() ? std::make_pair(true, DescriptionOfAction)
        : std::make_pair(false, CantDoAction);
}

// Executes the action
std::pair<bool, std::string> Action::DoAction() const {
    bool success = Functor ? Functor() : false;
    return success ? std::make_pair(true, DescriptionOfTakingAction)
        : std::make_pair(false, DescriptionOfTakingActionFail);
}


