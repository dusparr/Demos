#define NOMINMAX
#include <Windows.h>
#include "world.h"
#include "functors.h"
#include "stable_diffusion_client.h"
#include "character.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>

//  Global variables
std::atomic<bool> updateImage(false);
std::string currentCharacterImage;
sf::RenderWindow* _window;
//  Function to handle image rendering in a separate thread
void ImageWindow() {
    sf::RenderWindow window(sf::VideoMode(900, 1200), "Character View");
    
    sf::Texture texture;
    sf::Sprite sprite;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (updateImage) {
            // Load new image from file
            if (texture.loadFromFile(currentCharacterImage)) {
                sprite.setTexture(texture);
                updateImage = false;  // Reset flag after updating image
            }
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
        _window = &window;
    }
}

int main() {
    srand(time(nullptr));  //  Seed randomness (not used in light checks)

    //  Register all functors
    Functors::RegisterFunctors();

    //  Load actions, rooms, and characters from JSON
    Action::ConsumeAllActions({ "advancedactions.json" });
    Room::ConsumeRoomJson({ "advancedrooms.json" });
    Character::ConsumeCharacterJson({ "advancedcharacters.json" });

    //  Create the game world starting in the top-left room
    World gameWorld("room_entrance");
    StableDiffusionClient sdClient("http://192.168.50.105:7860/sdapi/v1/txt2img");  // Change if needed

    //  Launch the SFML image window in a separate thread
    std::thread imageThread(ImageWindow);

    std::string command;
    while (true) {
        //  Print room details every loop
        gameWorld.CurrentRoom.PrintRoomDetails();

        //  Get player input
        std::cout << "Enter command (move <room>, action <id>, look <character>, talk <character>, quit): ";
        std::getline(std::cin, command);
        if (!updateImage) {
            _window->setTitle("Game Window"); // Reset title when not looking at any character
        }
        if (command == "quit") break;

        if (command.rfind("move ", 0) == 0) {
            //  Handle room movement
            std::string roomID = command.substr(5);
            gameWorld.MoveToRoom(roomID);
            updateImage = false;
        }
        else if (command.rfind("action ", 0) == 0) {
            //  Execute an action
            std::string actionID = command.substr(7);
            gameWorld.ExecuteAction(actionID);
        }
        else if (command.rfind("look ", 0) == 0) {
            std::string characterID = command.substr(5);

            if (Character::AllCharacters.find(characterID) != Character::AllCharacters.end()) {
                Character& character = Character::AllCharacters[characterID];

                //  Check if the character is in the same room as the player
                if (character.CurrentRoomID != gameWorld.CurrentRoom.RoomKeyID) {
                    std::cerr << character.Name << " is not in this room.\n";
                    continue;
                }

                std::string prompt = character.GetPrompt();
                StableDiffusionRequest req;
                req = req.from_json_file("samplerJson.json");
                req.prompt = prompt;
                req.seed = -1;
                std::string imageBase64 = sdClient.GenerateImage(req);

                if (!imageBase64.empty()) {
                    std::string filename = characterID + ".png";
                    if (sdClient.SaveBase64Image(imageBase64, filename)) {
                        currentCharacterImage = filename;
                        updateImage = true;  // Notify the image window to update

                        //  Update the window title to reflect the character's name
                        _window->setTitle(character.Name);

                        std::cout << "Displaying new image for " << character.Name << "\n";
                    }
                    else {
                        std::cerr << "Failed to save image.\n";
                    }
                }
                else {
                    std::cerr << "Image generation failed.\n";
                }
            }
            else {
                std::cerr << "Character not found.\n";
            }
        }


    }

    imageThread.join();  //  Wait for the image window to close
    return 0;
}
