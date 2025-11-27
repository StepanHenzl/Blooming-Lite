#include "UserInstance.h"

#include <emscripten.h>

#include <chrono>

#include "../custom/Init.h"

EM_JS(void, InitAudioCache, (), 
{
    window.audioCache = {};
});

UserInstance::UserInstance()
{
    js_init_gl(ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution);

    auto TimeNow = std::chrono::system_clock::now();
    auto TimeFromEpochInMiliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(TimeNow.time_since_epoch()).count();

    RandomnessGenerator.seed(static_cast<unsigned long>(TimeFromEpochInMiliseconds));

    InitAudioCache();
}

UserInstance::~UserInstance()
{

}

//Engine tick. Handles non-fps based operations like rendering.
void UserInstance::Tick()
{
    double ThisFrameTime = emscripten_get_now();
    double DeltaTime = ThisFrameTime - LastFrameTime;
    LastFrameTime = ThisFrameTime;

    DeltaAccumulation += DeltaTime;

    while(DeltaAccumulation >= LogicalTickLengthInMS)
    {
        LogicalTick();
        DeltaAccumulation -= LogicalTickLengthInMS;
    }

    InitInstance.CallTick();

    AskForRenderedPixels();

    js_render_frame();
}

//Engine logical tick. Handles fps based operations that need delta time like physics.
void UserInstance::LogicalTick()
{
    InitInstance.CallLogicalTick();
}

void UserInstance::BeginPlay()
{
    InitInstance.BeginPlay();
}

//=================================================
//  Rendering
//=================================================

std::vector<uint32_t>* UserInstance::GetRenderedPixelsRef()
{
    return RenderedPixels;
}

//=================================================
//  Rendering Private
//=================================================

void UserInstance::AskForRenderedPixels()
{
    InitInstance.SendRenderedPixels(RenderedPixels);
}

//=================================================
//  Audio
//=================================================

void UserInstance::PlaySound(const char* Path)
{
    float Volume = GetVolume();
    EM_ASM_(
        {
        let pathStr = UTF8ToString($0);
        if (!window.audioCache) window.audioCache = {};

        if (!window.audioCache[pathStr]) 
        {
            window.audioCache[pathStr] = new Audio(pathStr);
        }
        let audio = window.audioCache[pathStr];
        audio.volume = $1;
        audio.currentTime = 0;
        audio.play();
    }, Path, Volume);
}

float UserInstance::GetVolume()
{
    return Volume;
}

//=================================================
//  Inputs
//=================================================

void UserInstance::RequestRegisterMouseUpSubscriber(std::function<void()> FuncCallback)
{
    InputHandlerInstance.RegisterMouseUpSubscriber(FuncCallback);
}

void UserInstance::RequestRegisterMouseDownSubscriber(std::function<void()> FuncCallback)
{
    InputHandlerInstance.RegisterMouseDownSubscriber(FuncCallback);
}

std::bitset<256>* UserInstance::GetPressedKeyListRef()
{
    return &PressedKeyList;
}


#ifdef __cplusplus
extern "C" 
{
#endif

    EMSCRIPTEN_KEEPALIVE
    uint32_t* get_pixel_buffer() 
    {
        return UserInstance::UserInstanceGet().GetRenderedPixelsRef()->data();
    }

#ifdef __cplusplus
}
#endif





