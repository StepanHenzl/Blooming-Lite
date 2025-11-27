#pragma once

#include "../src/Definitions.h"

#include "Render.h"
#include "TileBoard.h"
#include "MouseMovement.h"
#include "GameStateManager.h"

#include "API.h"

class UserInstance;


class Init
{
public:

    Init();

    void CallTick();
    void CallLogicalTick();
    void BeginPlay();

//=================================================
//  Rendering
//=================================================

    void SendRenderedPixels(std::vector<uint32_t>*& RenderedPixels);

//=================================================
//  Getters & Readers
//=================================================

    Render& GetRenderRef();
    TileBoard& GetTileBoardRef();
    GameStateManager& GetGameStateManagerRef();
    MouseMovement& GetMouseMovementRef();

    RenderAPI& GetRenderAPIRef();
    GameStateManagerAPI& GetGameStateManagerAPIRef();
    TileBoardAPI& GetTileBoardAPIRef();

private:

//=================================================
//  Instances & References
//=================================================

    Render RenderInstance;
    TileBoard TileBoardInstance;
    GameStateManager GameStateManagerInstance;
    MouseMovement MouseMovementInstance;

    RenderAPI RenderAPIInstance;
    GameStateManagerAPI GameStateManagerAPIInstance;
    TileBoardAPI TileBoardAPIInstance;
};