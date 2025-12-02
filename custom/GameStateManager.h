#pragma once

#include "../src/Definitions.h"

#include "CardEffects.h"

struct Card {
    std::string CardName;
    std::string CardText;
    const char* EvaluationText;
    bool IsActivatedOnPick;
    bool IsActivateOnRoundEnd;
};

class Init;

class GameStateManager{

public:
    GameStateManager(Init& InInitRef);

    void Tick();
    void BeginPlay();

//=================================================
//  Request State Change
//=================================================

    void RequestEvaluateRound();

//=================================================
//  Request Score State
//=================================================

    void RequestAddScore(int ScoreIn, int TileIndex);

//=================================================
//  State Change
//=================================================

    void LeftClickStateChanger();
    void PassToNextRound();

//=================================================
//  State Change Effects
//=================================================

    void AddBarrenSpawnAmount(int Amount);
    void AddRoundsPerDeadlineAmount(int Amount);
    void AddClicksPerRoundAmount(int Amount);

//=================================================
//  Score State
//=================================================

    void ChangeScoreValue(int ScoreIn);
    void ChangeDeadlineScore(int ScoreIn);

    void AddScoreMultiplerAmount(int Amount);
    void AddScoreAdditionalBonusesAmount(int Amount);
    void InvertScoreMultipler();

//=================================================
//  Effect Variable States
//=================================================

    void AddClicksRemainingAmount(int Amount);
    int ReadClickRemaining();

    const std::vector<int>& ReadOwnedCardsRef();
    const std::vector<Card>& ReadCardListRef();

    EffectConditions& GetEffectConditionsRef();

    bool ReadIsAbleToHighlightTiles();
    bool ReadIsAbleToChooseCard();

    CardEffects& GetCardEffectRef();
    const CardEffects& ReadCardEffect();

//=================================================
//  Logic For Determining The Active/Hovered Card
//=================================================

    void HighlightCard(int x, int y);



private:

//=================================================
//  Instances & References
//=================================================

    Init& InitRef;

    EffectConditions EffectConditionsInstance;
    CardEffects CardEffectsInstance;

//=================================================
//  State Change Private
//=================================================

    void TriggerDeadlineReward();
    void TriggerLose();
    void CheckDeadLineValue();
    void StartNewIteration();
    void EvaluateRound();
    void PassToDeadline();

//=================================================
//  State Change Effects Private
//=================================================

    void UpdateClicksRemainingAmountGUI(int PreviousClicksRemaining);
    void ResetRoundBasedVariables();

//=================================================
//  Score State Private
//=================================================

    void AddScore(int ScoreIn, int TileIndex);

//=================================================
//  State-Switch Variables
//=================================================

    bool IsAbleToHighlightTiles = true;
    bool IsAbleToEvaluateTiles = false;
    bool IsAbleToChooseCard = false;
    bool IsDone = false;
    bool IsPausedForDeadline = false;

    int EvaluationFramesBetweenSteps = 40;
    int EvaluationFramesCounter = 0;

    bool IsPointingAtCard = false;
    int HighlightedCardIndex = -1;
    int PreviouslyHighlightedCardIndex = -1;

//=================================================
//  State-Bound Variables
//=================================================

    int BaseClicksPerRound = 4;
    int BaseRoundsPerDeadline = 4;
    int BaseDeadlineValue = 10;
    int DeadlineValueIncrement[15] = {20, 100, 200, 350, 500, 800, 1400, 2000, 4000, 8000, 12000, 20000, 80000, 150000, 999999999};
    int DeadlineCounter = 0;

//=================================================
//  Active Game Variables
//=================================================

    int64_t Score = 0;

    int ScoreMultipler = 1;
    int ScoreAdditionalBonuses = 0;

    int ClicksPerRound = BaseClicksPerRound;
    int ClicksRemaining = ClicksPerRound;
    int RoundsPerDeadline = BaseRoundsPerDeadline;
    int RoundsRemaining = RoundsPerDeadline;
    int64_t DeadlineValue = BaseDeadlineValue;

    int AmountOfCardsToChoose = 3;

    int BaseBarrenSpawnPerRound = 3;
    int BarrenSpawnPerRound = BaseBarrenSpawnPerRound;

    std::vector<int> OwnedCards;
    std::vector<int> AvailableCards;
    std::vector<int> OfferedCards;

//=================================================
//  Extra
//=================================================

    const std::vector<Card> ListOfCards = 
    {
        {"BASE POINTS", "GET POINTS\nFOR EACH PLANT\nLEVEL", "POINTS FOR EACH LEVEL OF PLANTS", false, true},
        {"LAND BARGAIN", "GET SCORE POINT\nFOR EACH EMPTY\nTILE", "POINTS FOR EMPTY TILES", false, true},
        {"TREE TRIPS", "GET 500 POINTS\nFOR EACH GIANT\nTREE", "POINTS FOR GIANT TREES", false, true},
        {"MOSHREEM", "MUSHROOM BASE\nSCORE INCREASES BY\nONE FOR EACH\nMUSHROOM SCORED\nTHIS ROUND", "POINTS FOR MOSHREEMS", false, true},
        {"EVEN BIOME", "FOR EACH ADJACENT\nDIFFERENT LEVEL\nDOUBLE THIS TILE\nSCORE EXCEPT\nBARREN OR EMPTY", "POINTS FOR EVEN BIOME", false, true},
        {"BAD ME", "BARREN BASE SCORE\nBECOMES 5\nTREES BASE SCORE\nBECOMES NEGATIVE 50", "POINTS FOR BAD ME", false, true},
        {"ON EDGE", "ODD SCORES\nARE DOUBLED\nEVEN SCORES\nARE HALVED", "-", true, false},

        {"QUICK HAND", "CLICKS PER ROUND\nINCREASE BY TWO", "-", true, false},
        {"TOUCHE", "ONE IN THREE\nCHANCE CLICK IS\nNOT SPENT WHEN\nLEVELING PLANT", "-", true, false},
        {"WITCH CURSE", "CLICKS PER ROUND\nIS ALWAYS 1\nCLICK CAN GROW TO\nANY PLANT LEVEL", "-", true, false},
        {"DEMOLISH MAN", "CLICKING A PLANT\nHAS ONE IN TWO\nCHANCE TO DESTROY\nIT AND GAIN 80\nPOINTS", "-", true, false},

        {"SLOW YEAR", "ROUNDS INCREASE\nBY ONE", "-", true, false},

        {"DEEP FOREST", "EACH TREE SCORES\nBASE SCORE FOR\nEACH ADJECTED TREE", "POINTS FOR DEEP FOREST", false, true},
        {"GRASSHOPPER", "EACH GRASS SCORES\nAGAIN AND ALSO FOR\nADJACENT GRASS OF\nOTHER LENGTHS\nREPEAT UNTIL ALL\nLENGTHS REACHED", "POINTS FOR GRASSHOPERS", false, true},
        {"GOLDEN WASTE", "EACH BARREN TILE\nTRIGGERS BONUSES OF\nADJACENT NON BARREN\nTILES AGAIN", "ACTIVATING GOLDEN WASTE", false, true},

        {"INFECTION", "AT ROUND END\nMUSHROOMS EXPAND\nINTO ADJACENT\nEMPTY TILES", "EXPANDING INFECTION", false, true},
        {"SUN COVER", "AT ROUND END\nTREES TURN EMPTY OR\nFLOWER TILES INTO\nGRASS", "TREES COVERING SUN", false, true},

        {"LOST CAUSE", "INVERT THE SIGN OF\nALL GAINED SCORE", "-", true, false},
        {"GREEDY ME", "SCORE MULTIPLIER\nINCREASES BY ONE\nSPAWN TWO MORE\nBARREN EACH ROUND", "-", true, false},
        {"TAX COLLECTOR", "FOR EACH SCORE\nGAIN GET ONE\nEXTRA POINT", "-", true, false},

        {"CURSED GROVE", "TREES GIVES 50\nPOINT BUT EMPTY\nTILES ADJECTED TO\nTHEM -30", "POINTS FOR CURSED GROVE", false, true},
        {"END OF TIME", "TILES WILL NOT\nLEVEL UP\nAT THE END\nOF EACH ROUND", "-", true, false},
        {"SORE LAND", "INCREASE SCORE\nMULTIPLER BY\nONE BUT\nGET -5 SCORE\nFOR EACH\nNON-EMPTY LAND", "POINTS FOR SORE LAND", true, true}
    };
};