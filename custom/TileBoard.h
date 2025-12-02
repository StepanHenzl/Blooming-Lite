#pragma once

#include <vector>

#include "../src/Definitions.h"

class Init;

class Render;

class UserInstance;

class TileBoard
{

public:
    TileBoard(Init& InInitRef);

    void BeginPlay();

//=================================================
//  Tile Board Initialization
//=================================================

    void DrawEmptyTiles();

//=================================================
//  Round End Tile Effects
//=================================================

    void LevelUpTiles();
    bool EvaluateTiles();
    void TileGetScore(int TileIndex, int ScoreIn);
    void TriggerTileEffect(int TileIndex, int EffectIndex);
    void SpawnBarrenRandomly(int IdealAmountOfBarrenToSpawn);

//=================================================
//  Tiles Input Interaction
//=================================================

    void HighlightTile(double MouseX, double MouseY);
    void HandleLeftClickTile();
    void LeftMouseButtonReleased();

//=================================================
//  Getters & Readers
//=================================================

    int ReadTilesHeight();
    void ResetTileObjects();
    std::vector<int>& GetTilesInfectedThiTurnRef();

private:

//=================================================
//  Instances & References
//=================================================

    Init& InitRef;
    Render& RenderHelper;


//=================================================
//  Tiles Dimensions
//=================================================

    void GetTileNeighbors();
    int CalculateTileIndex(int X, int Y, int TileDimension);
    int CalculateStartIndexInDiagonal(int SumOfCoordinates, int TileDimension);
    void GetDistanceToClosestTile(double MouseX, double MouseY, int& BestIndexResult, int& SmallestDistanceResult);

    std::vector<std::vector<int>> TileNeighbors;
    std::vector<Vector2D<int>> CenterCoordOfTiles;

    static constexpr int TilesRows = 15;
    static constexpr int TilesCount = (TilesRows / 2 + 1) * (TilesRows / 2 + 1);

    static constexpr int TilesTopCornerX = ConfigSettings::GL_Width_Resolution/2;
    static constexpr int TilesTopCornerY = 50;

    static constexpr int GapBetweenTiles = 3;

//=================================================
//  Tiles Input Data
//=================================================

    bool IsMouseInRange(double MouseX, double MouseY);

    std::vector<int> TilesIndexesUpgradedWithDragClick;

    int IndexOfHighlightedTile = 0;

    bool IsPointingAtTile = false;
    bool IsHighlightSoundPlayed = false;

    static constexpr int LevelsPerBaseTileClick = 1;

//=================================================
//  Tiles Evaluation Data
//=================================================

    size_t EvaluationStepsCount = 0;

    std::vector<int> TilesInfectedThisTurn;

//=================================================
//  Tiles Data
//=================================================

    std::vector<int> TilesObjects;

    static constexpr int HighestPlantLevel = PlantLevel::BigTree;
    static constexpr int BarrenLevel = PlantLevel::Barren;
    static constexpr int LowestPlantLevel = PlantLevel::ShortGrass;

//=================================================
//  Round End Tile Data
//=================================================

    static constexpr int LevelsPerRoundEndLevelUp = 1;
};