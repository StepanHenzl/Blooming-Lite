#include <emscripten.h>
#include <emscripten/html5.h>

#include "UserInstance.h"

#include "Definitions.h"

static bool IsEngineStarted = false;

void tick_of_main() 
{

    static bool IsBeginPlayAlreadyTriggered = false;

    if(!IsEngineStarted){
        return;
    }

    if(!IsBeginPlayAlreadyTriggered){
        IsBeginPlayAlreadyTriggered = true;
        UserInstance::UserInstanceGet().BeginPlay();
    }

    UserInstance::UserInstanceGet().Tick();

}

//Called from js once DOM elements are ready.
EMSCRIPTEN_KEEPALIVE
extern "C" void start_game() 
{
    IsEngineStarted = true;
}

int main()
{
    emscripten_set_main_loop(tick_of_main, 0, 1);

    return 0;
}

