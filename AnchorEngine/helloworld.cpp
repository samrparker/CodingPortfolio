#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "../src/Engine.h"
#include <iostream>

using namespace engineSpace;

int main(int argc, const char* argv[]) {
    std::cout << "Hello, World!\n";
    std::cout << "Note: the key to press to log a message is the '0' (zero) key" << std::endl;
    std::cout << "and the key to press to exit is the esc (escape) key!" << std::endl;

    Engine newEngine;
    GraphicsManager* newGraphicsManager = new GraphicsManager();
    InputManager* newInputManager = new InputManager();
    ScriptManager* newScriptManager = new ScriptManager();
    SoundManager* newSoundManager = new SoundManager();

    newEngine.graphics = newGraphicsManager;
    newEngine.input = newInputManager;
    newEngine.scripting = newScriptManager;
    newEngine.soundMan = newSoundManager;

    newEngine.Startup();
    newEngine.graphics->sprites = {};
    //newEngine.graphics->LoadSprite("Blue pig", "D:/ENGINE/anchorEngine/assets/images/Blue Pig.png", 0.0f, 0.0f);


    newEngine.input->SetWindow(newEngine.graphics->GetWindow());
    newEngine.InitLua();

    newEngine.scripting->LoadScript("InitScript", "assets/scripts/setupscript.lua");
    newEngine.scripting->RunScript("InitScript");

    std::cout << "Done with init run!" << std::endl;

    newEngine.scripting->LoadScript("BluePig", "assets/scripts/bluepig.lua");
    //The callback function
    newEngine.RunGameLoop([&]() {

        newEngine.graphics->DrawSprites();
        newEngine.scripting->RunScript("GameLoopScript");

        });

    newEngine.Shutdown();

    return 0;
}