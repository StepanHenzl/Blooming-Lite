#include "Init.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../src/UserInstance.h"

Init::Init() :
    RenderInstance(),
    TileBoardInstance(*this),
    GameStateManagerInstance(*this),
    MouseMovementInstance(*this)
{
    RenderAPIInstance = 
    {
        [this](int X, int Y, bool IsFilled, int Color, int RenderLayer)
            { RenderInstance.RequestTileDraw(X, Y, IsFilled, Color, RenderLayer); },

        [this](int X, int Y, int TileIndex, int SpriteID)
            { RenderInstance.RequestDrawTileSprite(X, Y, TileIndex, SpriteID); },

        [this](int X, int Y, std::string Text, int TargetX, int TargetY, float TravelTime, int FontSize) 
            { RenderInstance.RequestFlyingText(X, Y, Text, TargetX, TargetY, TravelTime, FontSize); }
    };

    GameStateManagerAPIInstance = 
    {
        [this](int ScoreIn, int TileIndex) { GameStateManagerInstance.RequestAddScore(ScoreIn, TileIndex); },
        [this](int Amount) { GameStateManagerInstance.AddScoreMultiplerAmount(Amount); },
        [this]() { GameStateManagerInstance.InvertScoreMultipler(); },
        [this](int Amount) { GameStateManagerInstance.AddBarrenSpawnAmount(Amount); },
        [this](int Amount) { GameStateManagerInstance.AddScoreAdditionalBonusesAmount(Amount); },
        [this]() -> EffectConditions& { return GameStateManagerInstance.GetEffectConditionsRef(); },
        [this](int Amount) { GameStateManagerInstance.AddRoundsPerDeadlineAmount(Amount); },
        [this](int Amount) { GameStateManagerInstance.AddClicksPerRoundAmount(Amount); },
        [this]() -> const std::vector<int>& { return GameStateManagerInstance.ReadOwnedCardsRef(); },
        [this]() -> const std::vector<Card>& { return GameStateManagerInstance.ReadCardListRef(); }
    };

    TileBoardAPIInstance = {
        [this](int TileIndex, int EffectIndex) { TileBoardInstance.TriggerTileEffect(TileIndex, EffectIndex); },
        [this]() -> std::vector<int>& { return TileBoardInstance.GetTilesInfectedThiTurnRef(); },
        [this]() -> int { return TileBoardInstance.ReadTilesHeight(); }
    };

}



void Init::CallTick()
{
    MouseMovementInstance.CheckMouseMovement();

    GameStateManagerInstance.Tick();
    RenderInstance.Tick();
}

void Init::CallLogicalTick()
{
}

void Init::BeginPlay()
{
    RenderInstance.BeginPlay();
    TileBoardInstance.BeginPlay();
    GameStateManagerInstance.BeginPlay();
    MouseMovementInstance.BeginPlay();
}

//=================================================
//  Rendering
//=================================================

void Init::SendRenderedPixels(std::vector<uint32_t>*& RenderedPixels)
{
    RenderedPixels = RenderInstance.GetRenderedPixelsRef();
}

//=================================================
//  Getters & Readers
//=================================================

Render& Init::GetRenderRef()
{
    return RenderInstance;
}

TileBoard& Init::GetTileBoardRef()
{
    return TileBoardInstance;
}

GameStateManager& Init::GetGameStateManagerRef()
{
    return GameStateManagerInstance;
}

MouseMovement& Init::GetMouseMovementRef()
{
    return MouseMovementInstance;
}

RenderAPI& Init::GetRenderAPIRef()
{
    return RenderAPIInstance;
}

GameStateManagerAPI& Init::GetGameStateManagerAPIRef()
{
    return GameStateManagerAPIInstance;
}

TileBoardAPI& Init::GetTileBoardAPIRef()
{
    return TileBoardAPIInstance;
}



