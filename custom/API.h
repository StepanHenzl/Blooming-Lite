#pragma once

#include <functional>

struct RenderAPI
{
    std::function<void(int X, int Y, bool IsFilled, int Color, int RenderLayer)> RequestTileDraw;
    std::function<void(int X, int Y, int TileIndex, int SpriteID)> RequestDrawTileSprite;
    std::function<void(int X, int Y, std::string Text, int TargetX, int TargetY, float TravelTime, int FontSize)> RequestFlyingText;
};

struct EffectConditions;

struct Card;

struct GameStateManagerAPI
{
    std::function<void(int ScoreIn, int TileIndex)> RequestAddScore;
    std::function<void(int Amount)> AddScoreMultiplerAmount;
    std::function<void()> InvertScoreMultipler;
    std::function<void(int Amount)> AddBarrenSpawnAmount;
    std::function<void(int Amount)> AddScoreAdditionalBonusesAmount;
    std::function<EffectConditions&()> GetEffectConditionsRef;
    std::function<void(int Amount)> AddRoundsPerDeadlineAmount;
    std::function<void(int Amount)> AddClicksPerRoundAmount;
    std::function<const std::vector<int>&()> ReadOwnedCardsRef;
    std::function<const std::vector<Card>&()> ReadCardListRef;
};

struct TileBoardAPI
{
    std::function<void(int TileIndex, int EffectIndex)> TriggerTileEffect;
    std::function<std::vector<int>&()> GetTilesInfectedThiTurnRef;
    std::function<int()> ReadTilesHeight;
};