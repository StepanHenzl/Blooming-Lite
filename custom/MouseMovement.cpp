#include "MouseMovement.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../src/Definitions.h"
#include "../src/UserInstance.h"

#include "Init.h"

MouseMovement::MouseMovement(Init& InInitRef) :
    InitRef(InInitRef)
{
}

void MouseMovement::BeginPlay()
{
    UserInstance::UserInstanceGet().RequestRegisterMouseDownSubscriber([this] (){this->LeftMouseButtonPressed();});
    UserInstance::UserInstanceGet().RequestRegisterMouseUpSubscriber([this] (){this->LeftMouseButtonReleased();});
}

//=================================================
//  Mouse Movement
//=================================================

void MouseMovement::CheckMouseMovement()
{
    EMSCRIPTEN_RESULT Result;
    EmscriptenMouseEvent MouseEvent;

    Result = emscripten_get_mouse_status(&MouseEvent);

    if (Result != EMSCRIPTEN_RESULT_SUCCESS)
        return;

    double CanvasLeftOffset = 0;
    double CanvasTopOffset = 0;
    js_get_canvas_distance(&CanvasLeftOffset, &CanvasTopOffset);

    double CanvasX = MouseEvent.clientX - CanvasLeftOffset;
    double CanvasY = MouseEvent.clientY - CanvasTopOffset;

    double CanvasWidth;
    double CanvasHeight;
    js_get_canvas_size(&CanvasWidth, &CanvasHeight);

    double RatioX = CanvasWidth  / ConfigSettings::GL_Width_Resolution;
    double RatioY = CanvasHeight / ConfigSettings::GL_Height_Resolution;
    double MouseX = CanvasX / RatioX;
    double MouseY = CanvasY / RatioY;

    if(InitRef.GetGameStateManagerRef().ReadIsAbleToHighlightTiles())
    {
        InitRef.GetTileBoardRef().HighlightTile(MouseX, MouseY);
        if(IsLeftMouseButtonHold)
        {
            InitRef.GetTileBoardRef().HandleLeftClickTile();
        }
    }

    if(InitRef.GetGameStateManagerRef().ReadIsAbleToChooseCard())
    {
        InitRef.GetGameStateManagerRef().HighlightCard(MouseX, MouseY);
    }
}

//=================================================
//  Mouse Actions
//=================================================

void MouseMovement::LeftMouseButtonPressed()
{
    InitRef.GetTileBoardRef().HandleLeftClickTile();

    InitRef.GetGameStateManagerRef().LeftClickStateChanger();

    IsLeftMouseButtonHold = true;
}

void MouseMovement::RequestLeftMouseButtonUnpowered()
{
    LeftMouseButtonReleased();
}

void MouseMovement::LeftMouseButtonReleased()
{
    IsLeftMouseButtonHold = false;

    InitRef.GetTileBoardRef().LeftMouseButtonReleased();
}