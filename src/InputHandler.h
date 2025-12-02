#pragma once

#include <unordered_set>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "Definitions.h"

static std::unordered_set<int> PressedKeys;
static std::vector<std::function<void()>> MouseUpSubscribers;
static std::vector<std::function<void()>> MouseDownSubscribers;

EM_BOOL MouseMoveCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
EM_BOOL MouseDownCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
EM_BOOL MouseUpCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
EM_BOOL KeyDownCallback(int eventType, const EmscriptenKeyboardEvent *e, void *userData);
EM_BOOL KeyUpCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

class InputHandler
{

public:
    InputHandler();

    const std::unordered_set<int>& GetPressedKeysRef();

    void RegisterMouseUpSubscriber(std::function<void()> FuncCallback);
    void RegisterMouseDownSubscriber(std::function<void()> FuncCallback);

private:
};