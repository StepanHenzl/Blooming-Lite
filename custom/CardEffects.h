#pragma once

#include <array>
#include <vector>

#include "API.h"

#include "../src/Definitions.h"

#define INITIALIZE_EFFECT(Name) \
class Name##Effect : public Effect \
{ \
public: \
    void Trigger(CardEffectsContext& TriggerContext) override; \
} Name

class Init;

struct EffectValues
{
    //Land Bargain
    int LandBargainPlantLevel = 1;
    int LandBargainScoreReward = 1;

    //Tree trips
    int TreeTripsReward = 500;

    //Moshreem
    int MoshreemScoreCounter = 0;

    //Even Biome
    int EvenBiomeMinimumPlantLevel = 2;

    //Bad me
    int BarrenPointsReward = 5;
    int BadMeTreePenalty = -50;

    //Demolish man
    int RewardForDemolish = 80;

    //Cursed grove
    int RewardPerTreeGrove = 50;
    int PenalisationPerEmptyTileGrove = -30;

    //Quick Hand
    int QuickHandBonusClicks = 2;

    //Slow Year
    int SlowYearBonusRounds = 1;

    //Golden Waste
    int GoldenWasteEffectIndex = 7;

    //Sun Cover
    int SunCoverGrassVersionCount = 3;

    //Greedy Me
    int GreedyMeScoreMultipler = 1;
    int GreedyMeBarrenSpawnAmount = 2;

    //Tax Collector
    int TaxCollectorBonusScore = 1;

    //Sore land
    int SoreLandScorePointsPerNonEmptyTile = -5;
    int SoreLandScoreMultipler = 1;

    //Witch Curse
    int WitchCurseClicksAmount = 1;
};

struct CardEffectsContext 
{
    int TileIndex;
    int TileX;
    int TileY;
    std::vector<int>& TilesObjects;
    const std::vector<std::vector<int>>& TileNeighbors;
    const std::vector<Vector2D<int>>& CenterCoordOfTiles;
    EffectValues& EffectValuesInstance;
    const RenderAPI& RenderAPIRef;
    const GameStateManagerAPI& GameStateManagerAPIRef;
    const TileBoardAPI& TileBoardAPIRef;

    Init* InitRef;

    CardEffectsContext
    (
        int TileIndex, 
        int TileX, 
        int TileY, 
        std::vector<int>& TilesObjects,
        const std::vector<std::vector<int>>& TileNeighbors,
        const std::vector<Vector2D<int>>& CenterCoordOfTiles,
        EffectValues& EffectValuesInstance,
        const RenderAPI& RenderAPIRef, 
        const GameStateManagerAPI& GameStateManagerAPIRef,
        const TileBoardAPI& TileBoardAPIRef
    ) :
    TileIndex(TileIndex), 
    TileX(TileX), 
    TileY(TileY), 
    TilesObjects(TilesObjects),
    TileNeighbors(TileNeighbors),
    CenterCoordOfTiles(CenterCoordOfTiles),
    EffectValuesInstance(EffectValuesInstance),
    RenderAPIRef(RenderAPIRef), 
    GameStateManagerAPIRef(GameStateManagerAPIRef),
    TileBoardAPIRef(TileBoardAPIRef) 
    {
    }
};

class Effect 
{
public:
    virtual void Trigger(CardEffectsContext& TriggerContext) = 0;
    virtual ~Effect() = default;
protected:
    void TileScoreFeedback(CardEffectsContext& TriggerContext, int Score);
};

class CardEffects
{

public:
    CardEffects();

//=================================================
//  Instances & References
//=================================================

    EffectValues EffectVariables;

//=================================================
//  Getters & Readers
//=================================================

    const std::vector<Effect*>& ReadEffectsRef() const;

    EffectValues& GetEffectValues();
    const EffectValues& ReadEffectValues() const;

private:

//=================================================
//  Effects Triggable On Each Individual Tile.
//=================================================

    INITIALIZE_EFFECT(BaseScorePoint);
    INITIALIZE_EFFECT(LandBargain);
    INITIALIZE_EFFECT(TreeTips);
    INITIALIZE_EFFECT(Moshreem);
    INITIALIZE_EFFECT(EvenBiome);
    INITIALIZE_EFFECT(BadMe);
    INITIALIZE_EFFECT(OnEdge);
    INITIALIZE_EFFECT(QuickHand);
    INITIALIZE_EFFECT(Touche);
    INITIALIZE_EFFECT(WitchCurse);
    INITIALIZE_EFFECT(DemolishMan);
    INITIALIZE_EFFECT(SlowYear);
    INITIALIZE_EFFECT(DeepForest);
    INITIALIZE_EFFECT(Grasshopers);
    INITIALIZE_EFFECT(GoldenWaste);
    INITIALIZE_EFFECT(Infection);
    INITIALIZE_EFFECT(SunCover);
    INITIALIZE_EFFECT(LostCause);
    INITIALIZE_EFFECT(GreedyMe);
    INITIALIZE_EFFECT(TaxCollector);
    INITIALIZE_EFFECT(CursedGrove);
    INITIALIZE_EFFECT(EndOfTime);
    INITIALIZE_EFFECT(SoreLand);

    std::vector<Effect*> Effects;
};

struct EffectConditions 
{
    bool IsSorelandMultiplerApplied = false;
    bool IsOnEdgeActive = false;
    bool IsToucheEnabled = false;
    bool IsDemolishManEnabled = false;
    bool IsEndOfTimeActive = false;
    bool IsWitchCurseActive = false;
};
