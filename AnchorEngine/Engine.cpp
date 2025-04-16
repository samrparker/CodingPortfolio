#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Engine.h"

#include <iostream>

namespace engineSpace {

    void Engine::Startup() {
        Engine::graphics->Startup();
        Engine::scripting->Startup();
        Engine::soundMan->Startup();

        shouldExit = false;
    }

    void Engine::Shutdown() {
        Engine::graphics->Shutdown();
        Engine::soundMan->Shutdown();
    }

    //Add UpdateCallback to this as a parameter
    void Engine::RunGameLoop(const UpdateCallback& callback) {

        //Initializes the variables needed to calculate the fps
        auto prevTime = std::chrono::steady_clock::now();
        const auto frameTime = std::chrono::duration<double>(1. / 60.);


        while (true) {

            if (Engine::graphics->ShouldQuit() || shouldExit) {
                std::cout << "Exited SUCCcessfully" << std::endl;
                break;
            }

            //Calls the callback function
            callback();
            Engine::input->Update();


            // Makes it so the framerate is 60fps (probably)
            auto nowTime = std::chrono::steady_clock::now();
            auto deltaTime = nowTime - prevTime;
            prevTime = nowTime;

            auto deltaSeconds = std::chrono::duration<double>(deltaTime).count();
            Engine::soundMan->UpdateSounds(deltaSeconds);


            auto secondsToSleep = std::chrono::duration<double>(frameTime - deltaTime);
            std::this_thread::sleep_for(secondsToSleep);
        }
    }

    //NOTE: Call this AFTER Startup in helloworld!
    void Engine::InitLua() {
        sol::state& lua = scripting->GetLua();
        //Input functionality
        lua.set_function("KeyPressed", [&](const int keycode) { return input->KeyIsPressed(keycode); });

        //Graphics functionality
        lua.set_function("MoveSprite", [&](const string& spriteName, const float moveX, const float moveY) { graphics->MoveSprite(spriteName, moveX, moveY); });
        lua.set_function("AddSprite", [&](const string& spriteName, const string& path, float x, float y) { graphics->LoadSprite(spriteName, path, x, y);  });

        //Scripting functionality
        lua.set_function("AddScript", [&](const string& scriptName, const string& path) {scripting->LoadScript(scriptName, path); });
        lua.set_function("RunScript", [&](const string& scriptName) {scripting->RunScript(scriptName); });

        //Scripting types just the types for ECS
        lua.new_usertype<Position>("position",
            sol::constructors<Position()>(),
            "x", &Position::x,
            "y", &Position::y
        );

        //Sound functionality
        lua.set_function("AddSound", [&](const string& soundName, const string& path) {soundMan->LoadSound(soundName, path);  });
        lua.set_function("PlaySound", [&](const string soundName) {soundMan->PlaySound(soundName); });
        lua.set_function("StopSound", [&](const string soundName) {soundMan->StopSound(soundName); });

        //General stuff, quitting, etc.
        lua.set_function("Quit", [&]() { shouldExit = true; });

    }
}