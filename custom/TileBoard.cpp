#include "TileBoard.h"

#include "../src/UserInstance.h"

#include "Init.h"

#include "Sounds.h"

#include "GameStateManager.h"

TileBoard::TileBoard(Init& InInitRef) :
    InitRef(InInitRef),
    RenderHelper(InInitRef.GetRenderRef())
{
    TilesObjects.resize(TilesCount);
    std::fill(TilesObjects.begin(), TilesObjects.end(), 1);

    GetTileNeighbors();
}

void TileBoard::BeginPlay()
{
    DrawEmptyTiles();
}

//=================================================
//  Tile Board Initialization
//=================================================

//Draw the background grid of empty tiles row by row into diamond shape.
void TileBoard::DrawEmptyTiles()
{
    int TilesHeight = TilesParameters::TilesHeight;
    int HalfHeight = TilesRows / 2;
    int TileDimensionWithGap = TilesHeight + GapBetweenTiles;
    for(int i = 0; i < TilesRows; i++)
    {
        //Offset each row by half tiles height, cause diamond tiles overlap.
        int RelativeY = TilesTopCornerY + i * (TilesHeight/2 + GapBetweenTiles);
        int LeftMostX;
        int RowLength;

        if(i <= HalfHeight)
        {
            LeftMostX = TilesTopCornerX - i * ((TilesHeight/2 - 1) + GapBetweenTiles) - 1;
            RowLength = i + 1;
        }
        else
        {
            LeftMostX = TilesTopCornerX - (TilesRows - (i + 1)) * ((TilesHeight/2 - 1) + GapBetweenTiles) - 1;
            RowLength = TilesRows - i;
        }

        for(int j = 0; j < RowLength; j++)
        {
            RenderHelper.RequestTileDraw(LeftMostX + j * TileDimensionWithGap, RelativeY, false, ConfigSettings::Definition_Color, RenderLayer::PersistentTiles);
            CenterCoordOfTiles.push_back({(LeftMostX + j * TileDimensionWithGap), (RelativeY + TilesHeight/2 + 1)});
        }
    }
}

//=================================================
//  Round End Tile Effects
//=================================================

void TileBoard::LevelUpTiles()
{
    for(int i = 0; i < TilesCount; i++)
    {
        if(TilesObjects[i] > PlantLevel::EmptyTile && TilesObjects[i] < HighestPlantLevel)
        {
            TilesObjects[i] += LevelsPerRoundEndLevelUp;
            RenderHelper.RequestDrawTileSprite(CenterCoordOfTiles[i].X, CenterCoordOfTiles[i].Y, i, TilesObjects[i]);
            auto TileCoord = CenterCoordOfTiles[i];
            InitRef.GetRenderRef().RequestFlyingText(TileCoord.X - 50, TileCoord.Y, "^LEVEL UP", TileCoord.X - 50, TileCoord.Y - 20, 30, 1);
            Sounds::RequestSound(Sounds::LevelUp);
        }
    }
}

bool TileBoard::EvaluateTiles()
{
    bool IsEverythingDone = true;
    InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, 1);

    auto OwnedCards = InitRef.GetGameStateManagerRef().ReadOwnedCardsRef();

    while(EvaluationStepsCount < OwnedCards.size())
    {
        if(!InitRef.GetGameStateManagerRef().ReadCardListRef()[OwnedCards[EvaluationStepsCount]].IsActivateOnRoundEnd)
        {
            EvaluationStepsCount++;
            continue;
        }
        for(int i = 0; i < TilesCount; i++)
        {
            TriggerTileEffect(i, OwnedCards[EvaluationStepsCount]);
        }
        InitRef.GetRenderRef().RequestFlyingText(200, 100, InitRef.GetGameStateManagerRef().ReadCardListRef()[OwnedCards[EvaluationStepsCount]].EvaluationText, 200, 30, 120, 2);
        IsEverythingDone = false;
        EvaluationStepsCount++;
        return false;
    }

    EvaluationStepsCount = 0;
    InitRef.GetGameStateManagerRef().PassToNextRound();
    return IsEverythingDone;
}

void TileBoard::TileGetScore(int TileIndex, int ScoreIn)
{
    InitRef.GetRenderRef().RequestTileScoreText(CenterCoordOfTiles[TileIndex].X, CenterCoordOfTiles[TileIndex].Y, ScoreIn);
}

void TileBoard::TriggerTileEffect(int TileIndex, int EffectIndex)
{
    int TilesHeight = TilesParameters::TilesHeight;
    CardEffectsContext CardEffectsContext(
        TileIndex, 
        CenterCoordOfTiles[TileIndex].X, 
        CenterCoordOfTiles[TileIndex].Y - (TilesHeight/2 + 1), 
        TilesObjects, TileNeighbors, CenterCoordOfTiles, 
        InitRef.GetGameStateManagerRef().GetCardEffectRef().GetEffectValues(), 
        InitRef.GetRenderAPIRef(), InitRef.GetGameStateManagerAPIRef(), 
        InitRef.GetTileBoardAPIRef()
    );
    
    auto EffectRef = InitRef.GetGameStateManagerRef().ReadCardEffect().ReadEffectsRef()[EffectIndex];
    EffectRef->Trigger(CardEffectsContext);
}

void TileBoard::SpawnBarrenRandomly(int IdealAmountOfBarrenToSpawn)
{
    std::vector<int> CleanTilesIndexes;
    for(int i = 0; i < TilesCount; i++)
    {
        if(TilesObjects[i] == PlantLevel::EmptyTile)
        {
            CleanTilesIndexes.push_back(i);
        }
    }

    std::uniform_int_distribution<int> Distribution(1, 100);
    int AmountOfBarrenToSpawn = std::min(IdealAmountOfBarrenToSpawn, static_cast<int>(CleanTilesIndexes.size()));
    for(int i = 0; i < AmountOfBarrenToSpawn; i++)
    {
        int RandomIndexTile = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % CleanTilesIndexes.size();
        int X = CenterCoordOfTiles[CleanTilesIndexes[RandomIndexTile]].X;
        int Y = CenterCoordOfTiles[CleanTilesIndexes[RandomIndexTile]].Y;
        RenderHelper.RequestDrawTileSprite(X, Y, CleanTilesIndexes[RandomIndexTile], RenderLayer::PersistentTiles);
        TilesObjects[CleanTilesIndexes[RandomIndexTile]] = PlantLevel::Barren;
        CleanTilesIndexes.erase(CleanTilesIndexes.begin() + RandomIndexTile);
    }

}

//=================================================
//  Tiles Input Interaction
//=================================================

void TileBoard::HighlightTile(double MouseX, double MouseY)
{
    int TilesHeight = TilesParameters::TilesHeight;
    const Vector2D<int>& PreviouslyHighlithedTile = CenterCoordOfTiles[IndexOfHighlightedTile];

    RenderHelper.RequestTileDraw
    (
        PreviouslyHighlithedTile.X, 
        PreviouslyHighlithedTile.Y - (TilesHeight/2 + 1), 
        true, 
        ConfigSettings::Empty_Color, 
        RenderLayer::TransistentTiles
    );

    if(!IsMouseInRange(MouseX, MouseY))
    {
        IsPointingAtTile = false;
        return;
    }

    const int HalfOfTilesHeight = TilesHeight / 2;

    int BestIndex;
    int SmallestDistance;
    GetDistanceToClosestTile(MouseX, MouseY, BestIndex, SmallestDistance);

    if (BestIndex < 0)
    {
        IsPointingAtTile = false;
        return;
    }

    if(BestIndex != IndexOfHighlightedTile)
        IsHighlightSoundPlayed = false;

    const Vector2D<int>& Tile = CenterCoordOfTiles[BestIndex];

    if (SmallestDistance <= HalfOfTilesHeight)
    {
        RenderHelper.RequestTileDraw(Tile.X, Tile.Y - (TilesHeight/2 + 1), true, ConfigSettings::Definition_Color, RenderLayer::TransistentTiles);
        IndexOfHighlightedTile = BestIndex;
        IsPointingAtTile = true;
        if(!IsHighlightSoundPlayed){
            Sounds::RequestSound(Sounds::TileHoverSound);
            IsHighlightSoundPlayed = true;
        }
        return;
    }
    else
    {
        IsPointingAtTile = false;
    }

}

void TileBoard::HandleLeftClickTile()
{
    if(IsPointingAtTile && InitRef.GetGameStateManagerRef().ReadClickRemaining() > 0)
    {
        if(TilesObjects[IndexOfHighlightedTile] == BarrenLevel || TilesObjects[IndexOfHighlightedTile] >= HighestPlantLevel)
        {
            Sounds::RequestSound(Sounds::TileHoverSound);
            return;
        }

        if(
            std::find
            (
                TilesIndexesUpgradedWithDragClick.begin(), 
                TilesIndexesUpgradedWithDragClick.end(), 
                IndexOfHighlightedTile
            ) != TilesIndexesUpgradedWithDragClick.end()
        )
        {
            return;
        }

        if (TilesObjects[IndexOfHighlightedTile] > LowestPlantLevel)
        {
            if(InitRef.GetGameStateManagerRef().GetEffectConditionsRef().IsToucheEnabled)
            {
                std::uniform_int_distribution<int> Distribution(1, 100);
                int RandomConditionResult = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % 3;
                if(RandomConditionResult == 2)
                    InitRef.GetGameStateManagerRef().AddClicksRemainingAmount(1);
            }

            if(InitRef.GetGameStateManagerRef().GetEffectConditionsRef().IsDemolishManEnabled)
            {
                std::uniform_int_distribution<int> Distribution(1, 100);
                int RandomChance = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % 2;
                if(RandomChance == 1)
                {
                    TilesObjects[IndexOfHighlightedTile] = 0;
                    InitRef.GetGameStateManagerRef().RequestAddScore
                    (
                        InitRef.GetGameStateManagerRef().ReadCardEffect().ReadEffectValues().RewardForDemolish, 
                        IndexOfHighlightedTile
                    );
                }
            }
        }

        TilesObjects[IndexOfHighlightedTile] += LevelsPerBaseTileClick;

        //Witch Curse: Clicking on tile generate plant of random level. (Except empty or barren.)
        if (InitRef.GetGameStateManagerRef().GetEffectConditionsRef().IsWitchCurseActive)
        {
            std::uniform_int_distribution<int> Distribution(1, 100);
            int RandomPlantLevel = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % (HighestPlantLevel - 1);
            TilesObjects[IndexOfHighlightedTile] = RandomPlantLevel + LowestPlantLevel;
        }

        RenderHelper.RequestDrawTileSprite(CenterCoordOfTiles[IndexOfHighlightedTile].X, CenterCoordOfTiles[IndexOfHighlightedTile].Y, IndexOfHighlightedTile, TilesObjects[IndexOfHighlightedTile]);
        TilesIndexesUpgradedWithDragClick.push_back(IndexOfHighlightedTile);

        InitRef.GetGameStateManagerRef().AddClicksRemainingAmount(-1);
    }
}

void TileBoard::LeftMouseButtonReleased()
{
   TilesIndexesUpgradedWithDragClick.clear(); 
}

//=================================================
//  Tiles Dimensions
//=================================================

/* Compute the lowest index in the diagonal where a tile is situated.
*
*   Tiles are indexed in a grid as follows:
*
*       0
*      1 2
*     3 4 5
*   . . . . .
*
*   If the grid is rotated 45° to the left, the indexes are as follows:
*
*   0 2 5 .
*   1 4 .
*   3 .
*   .
*
*   Triangle increment of sum of tile coordinates is equal to the number of diagonals in the first column.
*   For example index 1 has coordinate x: 0, y: 1. x + y = 1. 1 * (1 + 1) / 2 -> 1
*   Same applies for index 2 -> x:1, y:0. x + y = 1, so triangle increment is 1.
*
*   0 1 3 .
*   1 3 .       =   CalculateStartIndexInDiagonal(Tile.x + Tile.y)
*   3 .
*   .
*
*   This applies until the grid is expanding. For the second half of the grid, a different approach has to be applied -
*   We can get the correct diagonal result for the second half by:
*       1) Get the difference of maximum potential tile coordinates from actual tile coordinates. (MaxX - Tile.X), (MaxY - Tile.Y).
*       2) Calculate triangle increment of the difference.
*       3) Subtract result from maximum tile index.
*
*   Then indexing follows simple rule:
*
*   0 + x   1 + x   3 + x   .
*   1 + x   3 + x   .
*   3 + x   .
*   .
*
*/
void TileBoard::GetTileNeighbors()
{
    TileNeighbors.resize(TilesCount);
    int TileDimension = TilesRows / 2 + 1;
    for(int y = 0; y < TileDimension; y++)
    {
        for(int x = 0; x < TileDimension; x++)
        {
            int Index = CalculateTileIndex(x, y, TileDimension);

            //Left
            if(x > 0)
                TileNeighbors[Index].push_back(CalculateTileIndex(x - 1, y, TileDimension));

            //Right
            if(x < (TileDimension - 1))
                TileNeighbors[Index].push_back(CalculateTileIndex(x + 1, y, TileDimension));

            //Top
            if(y > 0)
                TileNeighbors[Index].push_back(CalculateTileIndex(x, y - 1, TileDimension));

            //Bottom
            if(y < (TileDimension - 1))
                TileNeighbors[Index].push_back(CalculateTileIndex(x, y + 1, TileDimension));
        }
    }
}

//Explained above.
int TileBoard::CalculateTileIndex(int X, int Y, int TileDimension)
{
    int SumOfCoordinates = X + Y;
    int DiagonalIndex = CalculateStartIndexInDiagonal(SumOfCoordinates, TileDimension);
    int CompensationX;
    if(SumOfCoordinates < TileDimension)
    {
        CompensationX = X;
    }
    else
    {
        CompensationX = -((TileDimension - 1) - X);
    }
    return DiagonalIndex + CompensationX;
}

//Explained above.
int TileBoard::CalculateStartIndexInDiagonal(int SumOfCoordinates, int TileDimension)
{
    if(SumOfCoordinates < TileDimension)
    {
        return SumOfCoordinates * (SumOfCoordinates + 1) / 2;
    }
    else
    {
        int HighestIndex = (TilesCount - 1);
        int DiagonalNum = (TileDimension - 1) * 2 - SumOfCoordinates;
        int NegativeIndex = DiagonalNum * (DiagonalNum + 1) / 2;
        return HighestIndex - NegativeIndex;
    }
}

void TileBoard::GetDistanceToClosestTile(double MouseX, double MouseY, int& BestIndexResult, int& SmallestDistanceResult)
{
    int BestIndex = -1;
    int64_t SmallestDistance = std::numeric_limits<int64_t>::max();

    for (size_t i = 0; i < CenterCoordOfTiles.size(); ++i)
    {
        const Vector2D<int>& Tile = CenterCoordOfTiles[i];

        const int64_t Distance = llabs(Tile.X - static_cast<int64_t>(MouseX)) + llabs(Tile.Y - static_cast<int64_t>(MouseY));

        if (Distance < SmallestDistance)
        {
            SmallestDistance = Distance;
            BestIndex = static_cast<int>(i);
        }
    }

    BestIndexResult = BestIndex;
    SmallestDistanceResult = SmallestDistance;
}

//=================================================
//  Tiles Input Data
//=================================================

//Checks if the mouse is in a reasonable range from the tile grid to avoid wasting computational resources otherwise.
bool TileBoard::IsMouseInRange(double MouseX, double MouseY)
{
    double HalfHeightOfDiamondTiles = (TilesRows/2) * (TilesParameters::TilesHeight + GapBetweenTiles);

    double CenterOfTilesY = static_cast<double>(TilesTopCornerY) + HalfHeightOfDiamondTiles;
    double RelativeMouseY = std::abs(MouseY - static_cast<double>(CenterOfTilesY));

    if (RelativeMouseY > HalfHeightOfDiamondTiles)
    {
        return false;
    }

    //Calculate how far the MouseX can be based on the MouseY distance from the tiles center. (Radius.)
    double MaxAcceptableMouseX = HalfHeightOfDiamondTiles - RelativeMouseY;
    
    double CenterOfTilesX = TilesTopCornerX;
    double RelativeMouseX = std::abs(MouseX - CenterOfTilesX);

    if (RelativeMouseX > MaxAcceptableMouseX)
    {
        return false;
    }

    return true;
}

//=================================================
//  Getters & Readers
//=================================================

int TileBoard::ReadTilesHeight()
{
    return TilesParameters::TilesHeight;
}

void TileBoard::ResetTileObjects()
{
    std::fill(TilesObjects.begin(), TilesObjects.end(), 1);
}

std::vector<int>& TileBoard::GetTilesInfectedThiTurnRef()
{
    return TilesInfectedThisTurn;
}