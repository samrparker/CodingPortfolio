#include <thread>
#include <chrono>
#include <functional>

#include "GraphicsManager.h"
#include "InputManager.h"
#include "ScriptManager.h"
#include "SoundManager.h"
#include "UIManager.h"

namespace engineSpace {
    class Engine {
    public:
        GraphicsManager* graphics;
        InputManager* input;
        ScriptManager* scripting;
        SoundManager* soundMan;
        void Startup();

        void Shutdown();

        //Add UpdateCallback to this as a parameter
        typedef std::function<void()> UpdateCallback;
        void RunGameLoop(const UpdateCallback& callback);

        void InitLua();

        bool shouldExit;
    };
}
