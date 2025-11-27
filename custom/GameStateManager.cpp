#include "GameStateManager.h"

#include "Init.h"

#include "../src/UserInstance.h"

#include "Sounds.h"

GameStateManager::GameStateManager(Init& InInitRef) :
    InitRef(InInitRef)
{
}

void GameStateManager::Tick()
{
    if(IsAbleToEvaluateTiles)
    {
        if(EvaluationFramesCounter >= EvaluationFramesBetweenSteps)
        {
            IsAbleToEvaluateTiles = !InitRef.GetTileBoardRef().EvaluateTiles();
            EvaluationFramesCounter = 0;
        } 
        else
        {
            EvaluationFramesCounter++;
        }
    }
}

void GameStateManager::BeginPlay()
{
    InitRef.GetTileBoardRef().SpawnBarrenRandomly(BarrenSpawnPerRound);

    InitRef.GetRenderRef().DrawNewRoundsCounters(ClicksPerRound, RoundsPerDeadline);

    ChangeScoreValue(0);
    ChangeDeadlineScore(DeadlineValue);


    for(size_t i = 1; i < ListOfCards.size(); i++)
    {
        AvailableCards.push_back(i);
    }

    OwnedCards.push_back(0);
}

//=================================================
//  Request State Change
//=================================================

void GameStateManager::RequestEvaluateRound()
{
    EvaluateRound();
}

//=================================================
//  Request Score State
//=================================================

void GameStateManager::RequestAddScore(int ScoreIn, int TileIndex)
{
    AddScore(ScoreIn, TileIndex);
}

//=================================================
//  State Change
//=================================================

//Handles state changes which require left click to continue.
void GameStateManager::LeftClickStateChanger()
{
    if(IsPausedForDeadline)
    {
        IsPausedForDeadline = false;
        CheckDeadLineValue();
    }

    if(IsDone)
        //Reload the entire page to fully reset the WebAssembly game.
        emscripten_run_script("location.reload();");

    //Give player reward for finishing deadline and start new iteration.
    if(IsPointingAtCard)
    {
        IsPointingAtCard = false;

        std::vector<int>::iterator RemovedElementPosition = std::find(AvailableCards.begin(), AvailableCards.end(), OfferedCards[HighlightedCardIndex]);
        if (RemovedElementPosition != AvailableCards.end())
        {
            AvailableCards.erase(RemovedElementPosition);
        }

        OwnedCards.push_back(OfferedCards[HighlightedCardIndex]);
        if(ListOfCards[OfferedCards[HighlightedCardIndex]].IsActivatedOnPick)
        {
            InitRef.GetTileBoardRef().TriggerTileEffect(-1, OfferedCards[HighlightedCardIndex]);
        }

        InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, 2);
        IsAbleToChooseCard = false;

        InitRef.GetMouseMovementRef().RequestLeftMouseButtonUnpowered();

        StartNewIteration();
    }
}

void GameStateManager::PassToNextRound()
{
    RoundsRemaining -= 1;
    if(RoundsRemaining < 1)
    {
        InitRef.GetRenderRef().RequestErasePrimaryClickCounter(ClicksRemaining);

        InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, 0);
        PassToDeadline();
    } 
    else
    {
        int PreviousClicksRemaining = ClicksRemaining;
        ClicksRemaining = ClicksPerRound;
        if(EffectConditionsInstance.IsWitchCurseActive)
            ClicksRemaining = CardEffectsInstance.ReadEffectValues().WitchCurseClicksAmount;

        UpdateClicksRemainingAmountGUI(PreviousClicksRemaining);

        IsAbleToHighlightTiles = true;
    }

    ResetRoundBasedVariables();
}

//=================================================
//  State Change Private
//=================================================

//Offers std::min(AvailableCards.size(), 3) cards to player
void GameStateManager::TriggerDeadlineReward()
{
    int AmountOfCardsChosen = 0;
    int AmountOfAvailableCards = AvailableCards.size();
    std::vector<int> ListOfRandomlyGeneratedNumbers;
    IsAbleToChooseCard = true;
    OfferedCards.resize(0);
    std::uniform_int_distribution<int> Distribution(1, 500);
    for(int i = 0; i < AmountOfCardsToChoose; i++)
    {
        if(AmountOfAvailableCards < 1)
            continue;
        int RandomIndexInListOfAvailableCards = Distribution(UserInstance::UserInstanceGet().RandomnessGenerator) % AvailableCards.size();
        int RandomCardIndex = AvailableCards[RandomIndexInListOfAvailableCards];

        std::vector<int>::iterator RemovedElementPosition = std::find(AvailableCards.begin(), AvailableCards.end(), RandomCardIndex);
        if (RemovedElementPosition != AvailableCards.end())
        {
            AvailableCards.erase(RemovedElementPosition);
        }

        ListOfRandomlyGeneratedNumbers.push_back(RandomCardIndex);
        OfferedCards.push_back(RandomCardIndex);
        AmountOfAvailableCards--;
        AmountOfCardsChosen++;

        
        InitRef.GetRenderRef().DrawCard
        (
            PositionSpecialGUI::CardsPositionLeftX[i], 
            PositionSpecialGUI::CardPositionTopY, 
            ListOfCards[RandomCardIndex].CardName, 
            ListOfCards[RandomCardIndex].CardText
        );
    }

    for(int i = 0; i < AmountOfCardsChosen; i++)
    {
        AvailableCards.push_back(ListOfRandomlyGeneratedNumbers[i]);
    }
}

void GameStateManager::TriggerLose()
{
    //Erase all content in all layers.
    InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, RenderLayer::PersistentTiles);
    InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, RenderLayer::TransistentTiles);
    InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, RenderLayer::PersistentGUI);
    InitRef.GetRenderRef().EraseSpace(0, 0, ConfigSettings::GL_Width_Resolution, ConfigSettings::GL_Height_Resolution, RenderLayer::TransistentGUI);

    InitRef.GetRenderRef().WriteSentence
    (
        PositionSpecialGUI::PositionLoseText1X, 
        PositionSpecialGUI::PositionLoseText1Y, 
        TextSpecialGUI::LoseText1, 
        TextSpecialGUI::LoseTextFontSize1, 
        RenderLayer::PersistentGUI
    );

    InitRef.GetRenderRef().WriteSentence
    (
        PositionSpecialGUI::PositionLoseText2X, 
        PositionSpecialGUI::PositionLoseText2Y, 
        TextSpecialGUI::LoseText2, 
        TextSpecialGUI::LoseTextFontSize2, 
        RenderLayer::PersistentGUI
    );

    IsDone = true;
}

void GameStateManager::CheckDeadLineValue()
{
    InitRef.GetRenderRef().EraseText
    (
        PositionSpecialGUI::PauseTextLeftX, 
        PositionSpecialGUI::PauseTextTopY, 
        TextSpecialGUI::ClickToContinueText, 
        TextSpecialGUI::ClickToContinueFontSize, 
        RenderLayer::PersistentGUI
    );

    if(Score < DeadlineValue)
    {
        TriggerLose();
    } 
    else
    {
        TriggerDeadlineReward();
    }
}

void GameStateManager::StartNewIteration()
{
    DeadlineValue += DeadlineValueIncrement[DeadlineCounter];
    DeadlineCounter++;

    ChangeScoreValue(0);

    RoundsRemaining = RoundsPerDeadline;

    InitRef.GetRenderRef().DrawNewRoundsCounters(ClicksPerRound, RoundsPerDeadline);
    ClicksRemaining = ClicksPerRound;
    if(EffectConditionsInstance.IsWitchCurseActive)
        ClicksRemaining = 1;

    InitRef.GetRenderRef().RequestRewriteDeadlineScore(DeadlineValue - DeadlineValueIncrement[DeadlineCounter], DeadlineValue);

    InitRef.GetTileBoardRef().ResetTileObjects();

    IsAbleToHighlightTiles = true;

    InitRef.GetTileBoardRef().DrawEmptyTiles();

    InitRef.GetTileBoardRef().SpawnBarrenRandomly(BarrenSpawnPerRound);
}

void GameStateManager::EvaluateRound()
{
    IsAbleToHighlightTiles = false;

    //Enable tile effect evaluation for this frame cycle.
    IsAbleToEvaluateTiles = true;

    EvaluationFramesCounter = EvaluationFramesBetweenSteps;

    if(!EffectConditionsInstance.IsEndOfTimeActive)
        InitRef.GetTileBoardRef().LevelUpTiles();
}

void GameStateManager::PassToDeadline()
{
    InitRef.GetRenderRef().WriteSentence
    (
        PositionSpecialGUI::PauseTextLeftX, 
        PositionSpecialGUI::PauseTextTopY, 
        TextSpecialGUI::ClickToContinueText, 
        TextSpecialGUI::ClickToContinueFontSize, 
        RenderLayer::PersistentGUI
    );

    IsPausedForDeadline = true;
}

//=================================================
//  State Change Effects
//=================================================

void GameStateManager::AddBarrenSpawnAmount(int Amount)
{
    BarrenSpawnPerRound += Amount;
}

void GameStateManager::AddRoundsPerDeadlineAmount(int Amount)
{
    RoundsPerDeadline += Amount;
}

void GameStateManager::AddClicksPerRoundAmount(int Amount)
{
    ClicksPerRound += Amount;
}

//=================================================
//  State Change Effects Private
//=================================================

void GameStateManager::UpdateClicksRemainingAmountGUI(int PreviousClicksRemaining)
{
    InitRef.GetRenderRef().RequestClicksAmountLeftText(PreviousClicksRemaining, ClicksRemaining);

    int AmountOfNonActiveRoundsRemaining = RoundsRemaining - 1;
    InitRef.GetRenderRef().RequestEraseSecondaryClickCounter(ClicksPerRound, AmountOfNonActiveRoundsRemaining);

    InitRef.GetTileBoardRef().SpawnBarrenRandomly(BarrenSpawnPerRound);
}

void GameStateManager::ResetRoundBasedVariables()
{
    CardEffectsInstance.GetEffectValues().MoshreemScoreCounter = 0;

    InitRef.GetTileBoardAPIRef().GetTilesInfectedThiTurnRef().clear();
}

//=================================================
//  Score State
//=================================================

void GameStateManager::ChangeScoreValue(int ScoreIn)
{
    //Update score (previous vs new).
    InitRef.GetRenderRef().RequestRewriteScore(Score, ScoreIn);
    Score = ScoreIn;
}

void GameStateManager::ChangeDeadlineScore(int ScoreIn)
{
    //Update score (previous vs new).
    InitRef.GetRenderRef().RequestRewriteDeadlineScore(DeadlineValue, ScoreIn);
}

void GameStateManager::AddScoreMultiplerAmount(int Amount)
{
    ScoreMultipler += Amount;
}

void GameStateManager::AddScoreAdditionalBonusesAmount(int Amount)
{
    ScoreAdditionalBonuses += Amount;
}

void GameStateManager::InvertScoreMultipler()
{
    ScoreMultipler *= -1;
}

//=================================================
//  Score State Private
//=================================================

void GameStateManager::AddScore(int ScoreIn, int TileIndex)
{
    int FinalScore = ScoreIn;

    //OnEdge effect: odd scores double, even scores halve.
    if(EffectConditionsInstance.IsOnEdgeActive)
    {
        if((FinalScore % 2) != 0)
        {
            FinalScore *= 2;
        } 
        else 
        {
            FinalScore /= 2;
        }
    }

    FinalScore = (FinalScore * ScoreMultipler) + ScoreAdditionalBonuses;

    InitRef.GetTileBoardRef().TileGetScore(TileIndex, FinalScore);
    FinalScore += Score;
    ChangeScoreValue(FinalScore);
}

//=================================================
//  Effect Variable States
//=================================================

void GameStateManager::AddClicksRemainingAmount(int Amount)
{
    ClicksRemaining += Amount;

    if(ClicksRemaining < 1)
    {
        EvaluateRound();
    } 
    else
    {
        InitRef.GetRenderRef().RequestClicksAmountLeftText(ClicksRemaining - Amount, ClicksRemaining);
    }
}

int GameStateManager::ReadClickRemaining()
{
    return ClicksRemaining;
}

const std::vector<int>& GameStateManager::ReadOwnedCardsRef()
{
    return OwnedCards;
}

const std::vector<Card>& GameStateManager::ReadCardListRef()
{
    return ListOfCards;
}

EffectConditions& GameStateManager::GetEffectConditionsRef()
{
    return EffectConditionsInstance;
}

bool GameStateManager::ReadIsAbleToHighlightTiles()
{
    return IsAbleToHighlightTiles;
}

bool GameStateManager::ReadIsAbleToChooseCard()
{
    return IsAbleToChooseCard;
}

CardEffects& GameStateManager::GetCardEffectRef()
{
    return CardEffectsInstance;
}

const CardEffects& GameStateManager::ReadCardEffect()
{
    return CardEffectsInstance;
}

//=================================================
//  Logic For Determining The Active/Hovered Card
//=================================================

//Determine which card the player is pointing at.
void GameStateManager::HighlightCard(int x, int y)
{
    HighlightedCardIndex = -1;
    IsPointingAtCard = false;

    if(y < PositionSpecialGUI::CardPositionTopY || y > PositionSpecialGUI::CardPositionTopY + PositionSpecialGUI::CardHeight)
        return;

    for(int i = 0; i < AmountOfCardsToChoose; i++)
    {
        if(x >= PositionSpecialGUI::CardsPositionLeftX[i] && x < (PositionSpecialGUI::CardsPositionLeftX[i] + PositionSpecialGUI::CardWidth))
        {
            HighlightedCardIndex = i;
        }
    }

    if(HighlightedCardIndex != -1)
    {
        IsPointingAtCard = true;
    }

    if(HighlightedCardIndex != PreviouslyHighlightedCardIndex)
    {
        PreviouslyHighlightedCardIndex = HighlightedCardIndex;
    } 
    else
    {
        return;
    }

    if(HighlightedCardIndex == -1)
        return;

}