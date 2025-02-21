#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>

class Action {
public:
    std::string id;
    std::string DescriptionOfAction;
    std::string DescriptionOfTakingAction;
    std::string DescriptionOfTakingActionFail;
    std::string CantDoAction;



public:
    // Static dictionary to store all actions
    static std::unordered_map<std::string, Action> actionsDictionary;
    std::function<bool()> Functor;      // Determines if action succeeds
    std::function<bool()> CanDoFunctor; // Determines if action can be performed

    // Default constructor
    Action();

    // Constructor: Load an action from the preloaded dictionary
    Action(const std::string& actionID);

    // Static method to consume all actions from a list of JSON files
    static void ConsumeAllActions(const std::vector<std::string>& jsonFilePaths);

    // Determines if an action can be performed (uses CanDoFunctor)
    std::pair<bool, std::string> CanDoAction() const;

    // Function to execute the action
    std::pair<bool, std::string> DoAction() const;

private:
    Action(const std::string& actionID, const std::string& descAction,
        const std::string& descSuccess, const std::string& descFail, const std::string& cantDo);
};

#endif // ACTION_H
