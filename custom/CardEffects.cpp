#include "CardEffects.h"

#include "Init.h"

#include "Sounds.h"

#include <cstdio>

#include "../src/UserInstance.h"

//=================================================
//  Helper Functions.
//=================================================

bool IsTree(int PlantLevel)
{
    return 
    (
        PlantLevel == PlantLevel::SmallTree  || 
        PlantLevel == PlantLevel::MediumTree || 
        PlantLevel == PlantLevel::BigTree
    );
}

bool IsGrass(int PlantLevel)
{
    return 
    (
        PlantLevel == PlantLevel::ShortGrass  || 
        PlantLevel == PlantLevel::MediumGrass || 
        PlantLevel == PlantLevel::LongGrass
    );
}

bool IsFlower(int PlantLevel)
{
    return 
    (
        PlantLevel == PlantLevel::SmallFlower  || 
        PlantLevel == PlantLevel::MediumFlower || 
        PlantLevel == PlantLevel::BigFlower
    );
}

//Award score and provide visual feedback on tile.
void Effect::TileScoreFeedback(CardEffectsContext& TriggerContext, int Score)
{
    int TileIndex = TriggerContext.TileIndex;
    int TileX = TriggerContext.TileX;
    int TileY = TriggerContext.TileY;

    TriggerContext.RenderAPIRef.RequestTileDraw(TileX, TileY, true, ConfigSettings::Definition_Color, RenderLayer::TransistentTiles);
    TriggerContext.GameStateManagerAPIRef.RequestAddScore(Score, TileIndex);
}

CardEffects::CardEffects()
{
    //Initialize list of effects so that we can reference effects via index.
    Effects = 
    {
        &BaseScorePoint,
        &LandBargain,
        &TreeTips,
        &Moshreem,
        &EvenBiome,
        &BadMe,
        &OnEdge,
        &QuickHand,
        &Touche,
        &WitchCurse,
        &DemolishMan,
        &SlowYear,
        &DeepForest,
        &Grasshopers,
        &GoldenWaste,
        &Infection,
        &SunCover,
        &LostCause,
        &GreedyMe,
        &TaxCollector,
        &CursedGrove,
        &EndOfTime,
        &SoreLand
    };
}

//=================================================
// Getters & Readers
//=================================================

const std::vector<Effect*>& CardEffects::ReadEffectsRef() const
{
    return Effects;
}

const EffectValues& CardEffects::ReadEffectValues() const
{
    return EffectVariables;
}

EffectValues& CardEffects::GetEffectValues()
{
    return EffectVariables;
}

//=================================================
//  Effects Triggable On Each Individual Tile.
//=================================================

//Award base score based on plant level on tile.
void CardEffects::BaseScorePointEffect::Trigger(CardEffectsContext& TriggerContext)
{
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(TilesObjects[TileIndex] == PlantLevel::EmptyTile)
        return;

    TileScoreFeedback(TriggerContext, TilesObjects[TileIndex] - 1);
}

//Award LandBargainScoreReward score if the tile is empty.
void CardEffects::LandBargainEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(TilesObjects[TileIndex] != EffectVariables.LandBargainPlantLevel)
        return;

    TileScoreFeedback(TriggerContext, EffectVariables.LandBargainScoreReward);
}

//Award TreeTripsReward score if the tile is BigTree.
void CardEffects::TreeTipsEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(TilesObjects[TileIndex] != PlantLevel::BigTree)
        return;

    TileScoreFeedback(TriggerContext, EffectVariables.TreeTripsReward);
}

//Award base score plus one for each mushroom already scored this round, if the tile is a mushroom.
void CardEffects::MoshreemEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(TilesObjects[TileIndex] != PlantLevel::Mushroom)
        return;

    TileScoreFeedback(TriggerContext, EffectVariables.MoshreemScoreCounter);
    EffectVariables.MoshreemScoreCounter++;
}

//Award base score multiplied by the number of different flora types among neighboring tiles, if this tile is flora.
void CardEffects::EvenBiomeEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;

    if(TilesObjects[TileIndex] < EffectVariables.EvenBiomeMinimumPlantLevel)
        return;

    int Multipler = 1;
    std::vector<int> FloraList;

    //Reserve space for index tile and its neighbours.
    FloraList.reserve(TileNeighbors[TileIndex].size() + 1);
    FloraList.push_back(TilesObjects[TileIndex]);

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        int NeighbourFauna = TilesObjects[TileNeighbors[TileIndex][i]];

        if(NeighbourFauna < EffectVariables.EvenBiomeMinimumPlantLevel)
        {
            continue;
        }

        std::vector<int>::iterator IndexOfSameFauna = std::find(FloraList.begin(), FloraList.end(), NeighbourFauna);

        if(IndexOfSameFauna == FloraList.end())
        {
            FloraList.push_back(NeighbourFauna);
            Multipler++;
        }
    }

    TileScoreFeedback(TriggerContext, (TilesObjects[TileIndex] - 1) * Multipler);
    return;
}

//Award BarrenPointsReward if the tile is barren, or apply BadMeTreePenalty if the tile is a tree.
void CardEffects::BadMeEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(TilesObjects[TileIndex] == PlantLevel::Barren)
    {
        TileScoreFeedback(TriggerContext, EffectVariables.BarrenPointsReward);
    }

    if(IsTree(TilesObjects[TileIndex]))
    {
        TriggerContext.GameStateManagerAPIRef.RequestAddScore(EffectVariables.BadMeTreePenalty, TileIndex);
    }
}

//Enables the OnEdge condition, which doubles odd scores and halves even scores.
void CardEffects::OnEdgeEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsOnEdgeActive = true;
}

//Adds QuickHandBonusClicks clicks per round
void CardEffects::QuickHandEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    TriggerContext.GameStateManagerAPIRef.AddClicksPerRoundAmount(EffectVariables.QuickHandBonusClicks);
}

//Enables a chance to avoid spending a click when leveling up a tile.
void CardEffects::ToucheEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsToucheEnabled = true;
}

//Allows clicks to place a random plant level, but reduces clicks per round to WitchCurseClicksAmount period.
void CardEffects::WitchCurseEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsWitchCurseActive = true;
}

//Gives a chance to demolish a plant instead and gain RewardForDemolish points.
void CardEffects::DemolishManEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsDemolishManEnabled = true;
}

//Adds SlowYearBonusRounds extra rounds each deadline.
void CardEffects::SlowYearEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    TriggerContext.GameStateManagerAPIRef.AddRoundsPerDeadlineAmount(EffectVariables.SlowYearBonusRounds);
}

//If the tile is a tree, award the base score for each neighboring tree.
void CardEffects::DeepForestEffect::Trigger(CardEffectsContext& TriggerContext)
{
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;

    if(!IsTree(TilesObjects[TileIndex]))
    {
        return;
    }

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        if(IsTree(TilesObjects[TileIndex]))
        {
            TileScoreFeedback(TriggerContext, TilesObjects[TileIndex] - 1);
        }
    }
}

//If the tile is grass, award base score and then chain to neighboring grass tiles of different lengths until no new length is found.
void CardEffects::GrasshopersEffect::Trigger(CardEffectsContext& TriggerContext)
{
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;
    const std::vector<Vector2D<int>>& CenterCoordOfTiles = TriggerContext.CenterCoordOfTiles;

    if(IsGrass(TilesObjects[TileIndex]))
    {
        TileScoreFeedback(TriggerContext, TilesObjects[TileIndex] - 1);
    }

    std::vector<int> GrassSizesAwarded;

    //Reserve space for index tile and its neighbours.
    GrassSizesAwarded.reserve(TileNeighbors[TileIndex].size() + 1);
    GrassSizesAwarded.push_back(TilesObjects[TileIndex]);

    //Check for any neighbor grass of different size.
    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        int NeighbourIndex = TileNeighbors[TileIndex][i];

        std::vector<int>::iterator IndexOfSameGrassLength = std::find(GrassSizesAwarded.begin(), GrassSizesAwarded.end(), TilesObjects[TileIndex]);
        if (IndexOfSameGrassLength == GrassSizesAwarded.end() && IsGrass(TilesObjects[TileIndex])) 
        {
            int TileX = CenterCoordOfTiles[TileNeighbors[TileIndex][i]].X;
            int TileY = CenterCoordOfTiles[TileNeighbors[TileIndex][i]].Y - (TriggerContext.TileBoardAPIRef.ReadTilesHeight()/2 + 1);
            TriggerContext.RenderAPIRef.RequestTileDraw(TileX, TileY, true, ConfigSettings::Definition_Color, RenderLayer::TransistentTiles);
            TriggerContext.GameStateManagerAPIRef.RequestAddScore((TilesObjects[NeighbourIndex] - 1), NeighbourIndex);
            GrassSizesAwarded.push_back(TilesObjects[NeighbourIndex]);

            //Check if neighbor grass have its own neighbor of different size.
            for(size_t j = 0; j < TileNeighbors[NeighbourIndex].size(); j++)
            {
                NeighbourIndex = TileNeighbors[NeighbourIndex][j];
                IndexOfSameGrassLength = std::find(GrassSizesAwarded.begin(), GrassSizesAwarded.end(), TilesObjects[TileIndex]);

                if (IndexOfSameGrassLength == GrassSizesAwarded.end() && IsGrass(TilesObjects[TileIndex])) 
                {
                    TileX = CenterCoordOfTiles[NeighbourIndex].X;
                    TileY = CenterCoordOfTiles[NeighbourIndex].Y - (TriggerContext.TileBoardAPIRef.ReadTilesHeight()/2 + 1);
                    TriggerContext.RenderAPIRef.RequestTileDraw(TileX, TileY, true, ConfigSettings::Definition_Color, RenderLayer::TransistentTiles);
                    TriggerContext.GameStateManagerAPIRef.RequestAddScore((TilesObjects[NeighbourIndex] - 1), TileNeighbors[NeighbourIndex][j]);
                    break;
                }
            }
            break;
        }
    }
}

//If tile is barren, trigger all unlocked round based effects of neighbor tiles, except this one.
void CardEffects::GoldenWasteEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;

    if(TilesObjects[TileIndex] != PlantLevel::Barren)
    {
        return;
    }

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        if(TilesObjects[TileNeighbors[TileIndex][i]] == PlantLevel::Barren || TilesObjects[TileNeighbors[TileIndex][i]] == PlantLevel::EmptyTile)
            continue;

        for(size_t j = 0; j < TriggerContext.GameStateManagerAPIRef.ReadOwnedCardsRef().size(); j++)
        {
            if(j == static_cast<size_t>(EffectVariables.GoldenWasteEffectIndex))
                continue;

            if(!TriggerContext.GameStateManagerAPIRef.ReadCardListRef()[TriggerContext.GameStateManagerAPIRef.ReadOwnedCardsRef()[j]].IsActivateOnRoundEnd)
                continue;

            TriggerContext.TileBoardAPIRef.TriggerTileEffect(TileNeighbors[TileIndex][i], TriggerContext.GameStateManagerAPIRef.ReadOwnedCardsRef()[j]);
        }
    }
}

//If the tile is a mushroom and a neighbor is empty, spread into the first available empty neighbor.
void CardEffects::InfectionEffect::Trigger(CardEffectsContext& TriggerContext)
{
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;
    const std::vector<Vector2D<int>>& CenterCoordOfTiles = TriggerContext.CenterCoordOfTiles;
    std::vector<int>& TilesInfectedThisTurn = TriggerContext.TileBoardAPIRef.GetTilesInfectedThiTurnRef();

    if(TilesObjects[TileIndex] != PlantLevel::Mushroom)
    {
        return;
    }

    std::vector<int>::iterator IndexPotentionallyInfested = std::find(TilesInfectedThisTurn.begin(), TilesInfectedThisTurn.end(), TileIndex);
    if(IndexPotentionallyInfested != TilesInfectedThisTurn.end())
        return;

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        if(TilesObjects[TileNeighbors[TileIndex][i]] == PlantLevel::EmptyTile)
        {
            TilesObjects[TileNeighbors[TileIndex][i]] = PlantLevel::Mushroom;
            auto TileCoord = CenterCoordOfTiles[TileNeighbors[TileIndex][i]];
            TriggerContext.RenderAPIRef.RequestDrawTileSprite(TileCoord.X, TileCoord.Y, TileNeighbors[TileIndex][i], PlantLevel::Mushroom);
            TilesInfectedThisTurn.push_back(TileNeighbors[TileIndex][i]);
            TriggerContext.RenderAPIRef.RequestFlyingText(TileCoord.X - 50, TileCoord.Y, "^INFECTED UP", TileCoord.X - 50, TileCoord.Y - 20, 30, 1);
            Sounds::RequestSound(Sounds::LevelUp);
            break;
        }
    }
}

//If the tile is a tree, convert all neighboring tiles that are flowers or empty into grass of random length.
void CardEffects::SunCoverEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;
    const std::vector<Vector2D<int>>& CenterCoordOfTiles = TriggerContext.CenterCoordOfTiles;

    if(!IsTree(TilesObjects[TileIndex]))
    {
        return;
    }

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        if(TilesObjects[TileNeighbors[TileIndex][i]] == PlantLevel::EmptyTile || IsFlower(TilesObjects[TileNeighbors[TileIndex][i]]))
        {
            std::uniform_int_distribution<int> Distribution(1, 100);
            int RandomGrassLevel = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % EffectVariables.SunCoverGrassVersionCount;

            int FinalTileLevel = RandomGrassLevel + PlantLevel::ShortGrass;
            TilesObjects[TileNeighbors[TileIndex][i]] = FinalTileLevel;
            auto TileCoord = CenterCoordOfTiles[TileNeighbors[TileIndex][i]];
            TriggerContext.RenderAPIRef.RequestDrawTileSprite(TileCoord.X, TileCoord.Y, TileNeighbors[TileIndex][i], FinalTileLevel);
            TriggerContext.RenderAPIRef.RequestFlyingText(TileCoord.X - 50, TileCoord.Y, "^GRASSED UP", TileCoord.X - 50, TileCoord.Y - 20, 30, 1);
            Sounds::RequestSound(Sounds::LevelUp);
        }
    }
}

//Enables score inversion.
void CardEffects::LostCauseEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.InvertScoreMultipler();
}

//Increases the score multiplier by GreedyMeScoreMultiplier and spawns GreedyMeBarrenSpawnAmount additional barren tiles per round.
void CardEffects::GreedyMeEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    TriggerContext.GameStateManagerAPIRef.AddScoreMultiplerAmount(EffectVariables.GreedyMeScoreMultipler);
    TriggerContext.GameStateManagerAPIRef.AddBarrenSpawnAmount(EffectVariables.GreedyMeBarrenSpawnAmount);
}

//Adds TaxCollectorBonusScore to every score gain.
void CardEffects::TaxCollectorEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    TriggerContext.GameStateManagerAPIRef.AddScoreAdditionalBonusesAmount(EffectVariables.TaxCollectorBonusScore);
}

//Award RewardPerTreeGrove if the tile is a tree, and apply PenalisationPerEmptyTileGrove for each empty neighboring tile.
void CardEffects::CursedGroveEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;
    const std::vector<std::vector<int>>& TileNeighbors = TriggerContext.TileNeighbors;

    if(!IsTree(TilesObjects[TileIndex]))
    {
        return;
    }

    TileScoreFeedback(TriggerContext, EffectVariables.RewardPerTreeGrove);

    for(size_t i = 0; i < TileNeighbors[TileIndex].size(); i++)
    {
        if(TilesObjects[TileNeighbors[TileIndex][i]] == PlantLevel::EmptyTile)
        {
            TriggerContext.GameStateManagerAPIRef.RequestAddScore(EffectVariables.PenalisationPerEmptyTileGrove, TileIndex);
        }
    }
}

//Disables automatic plant leveling at the end of the round.
void CardEffects::EndOfTimeEffect::Trigger(CardEffectsContext& TriggerContext)
{
    TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsEndOfTimeActive = true;
}

//Adds SoreLandScoreMultiplier to the score multiplier and apply SoreLandScorePointsPerNonEmptyTile if the tile is not empty.
void CardEffects::SoreLandEffect::Trigger(CardEffectsContext& TriggerContext)
{
    auto& EffectVariables = TriggerContext.EffectValuesInstance;
    std::vector<int>& TilesObjects = TriggerContext.TilesObjects;
    int TileIndex = TriggerContext.TileIndex;

    if(!TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsSorelandMultiplerApplied)
    {
        TriggerContext.GameStateManagerAPIRef.AddScoreMultiplerAmount(EffectVariables.SoreLandScoreMultipler);
        TriggerContext.GameStateManagerAPIRef.GetEffectConditionsRef().IsSorelandMultiplerApplied = true;
        return;
    }

    if(TilesObjects[TileIndex] == PlantLevel::EmptyTile)
    {
        return;
    }

   TriggerContext.GameStateManagerAPIRef.RequestAddScore(EffectVariables.SoreLandScorePointsPerNonEmptyTile, TileIndex);
}