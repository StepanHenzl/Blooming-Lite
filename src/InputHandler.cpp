#include "InputHandler.h"

#include "UserInstance.h"

#include "../custom/Init.h"

EM_BOOL MouseMoveCallback(int eventType, const EmscriptenMouseEvent* e, void* userData)
{
    return EM_TRUE;
}


EM_BOOL MouseDownCallback(int eventType, const EmscriptenMouseEvent* e, void* userData)
{

    PressedKeys.insert(e->button);

    for(auto& Subscriber : MouseDownSubscribers){
        Subscriber();
    }


    return EM_TRUE;
}

EM_BOOL MouseUpCallback(int eventType, const EmscriptenMouseEvent* e, void* userData)
{

    if(PressedKeys.find(e->button) != PressedKeys.end())
        PressedKeys.erase(e->button);

    for(auto& Subscriber : MouseUpSubscribers){
        Subscriber();
    }

    return EM_TRUE;
}


EM_BOOL KeyDownCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{
    
    PressedKeys.insert(e->keyCode);

    return EM_TRUE;
}

EM_BOOL KeyUpCallback(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{

    if(PressedKeys.find(e->keyCode) != PressedKeys.end())
        PressedKeys.erase(e->keyCode);

    return EM_TRUE;
}

InputHandler::InputHandler()
{
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, KeyDownCallback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, KeyUpCallback);

    emscripten_set_mousemove_callback("#canvas", nullptr, EM_FALSE, MouseMoveCallback);
    emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, MouseDownCallback);
    emscripten_set_mouseup_callback("#canvas", nullptr, EM_TRUE, MouseUpCallback);

    EM_ASM({
        var canvas = Module['canvas'];
        canvas.setAttribute('tabindex', '0');
        canvas.style.outline = 'none';

        let overlay = document.createElement('div');
        overlay.style.position = 'fixed';
        overlay.style.top = '0';
        overlay.style.left = '0';
        overlay.style.width = '100vw';
        overlay.style.height = '100vh';
        overlay.style.background = 'black';
        overlay.style.zIndex = '9999';
        overlay.style.display = 'flex';
        overlay.style.alignItems = 'center';
        overlay.style.justifyContent = 'center';
        overlay.style.color = 'white';
        overlay.style.fontSize = '240px';
        overlay.style.fontFamily = 'sans-serif';
        overlay.style.cursor = 'pointer';
        overlay.innerText = 'Click to Start';

        overlay.addEventListener('click', () => {
            canvas.focus();
            document.body.removeChild(overlay);
        });

        document.body.appendChild(overlay);
    });
}

const std::unordered_set<int>& InputHandler::GetPressedKeysRef()
{
    return PressedKeys;
}

void InputHandler::RegisterMouseUpSubscriber(std::function<void()> FuncCallback)
{
    MouseUpSubscribers.push_back(FuncCallback);
}

void InputHandler::RegisterMouseDownSubscriber(std::function<void()> FuncCallback)
{
    MouseDownSubscribers.push_back(FuncCallback);
}