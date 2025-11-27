#pragma once

#include <unordered_map>

#include "../src/Definitions.h"

#include "TileDraw.h"

struct FloatingText 
{
    std::string Text;
    float PositionX;
    float PositionY;
    int TargetX;
    int TargetY;
    float SpeedX;
    float SpeedY;
    float LifeTime;
    int FontSize;
};

class Render
{
public:
    Render();

    void Tick();
    void BeginPlay();

//=================================================
//  Request Draw
//=================================================    

    void RequestTileDraw(int X, int Y, bool IsFilled, int Color, int RenderLayer);
    void RequestDrawTileSprite(int X, int Y, int TileIndex, int SpriteID);

//=================================================
//  Request Generic Text
//=================================================    

    void RequestTileScoreText(int StartX, int StartY, int ScoreIn);
    void RequestRewriteScore(int OldScore, int NewScore);
    void RequestFlyingText(int X, int Y, std::string Text, int TargetX, int TargetY, float TravelTime, int FontSize);

//=================================================
//  Request Counters Text
//=================================================   

    void RequestClicksAmountLeftText(int ClicksRemainingBefore, int ClicksRemainingNow);
    void RequestErasePrimaryClickCounter(int ClicksRemainingBefore);
    void RequestEraseSecondaryClickCounter(int ClicksRemainingBefore, int CounterIndex);
    void RequestRewriteDeadlineScore(int OldScore, int NewScore);

//=================================================
//  Draw & Erase Draw
//================================================= 

    void DrawCard(int X, int Y, std::string CardName, std::string CardText);
    void EraseSpace(int X, int Y, int Width, int Height, int RenderLayer);

//=================================================
//  Text & Erase Text
//=================================================   

    void EraseText(int X, int Y, std::string Text, int FontSize, int RenderLayer);
    void DrawLetter(int X, int Y, std::vector<Vector2D<uint8_t>> LetterBitmap, int FontSize, int RenderLayer);
    void WriteSentence(int X, int Y, std::string Text, int FontSize, int RenderLayer);
    
//=================================================
//  Counters Text
//=================================================  

    void DrawNewRoundsCounters(int ClicksPerRound, int RoundsPerDeathLine);

//=================================================
//  Getters
//=================================================

    std::vector<uint32_t>* GetRenderedPixelsRef();


private:

//=================================================
//  Instances & references
//=================================================

    TileDraw TileDrawInstance;


//=================================================
//  Tick-Based Methods
//=================================================

    void CombineRenderingLayers();
    void UpdateTransistentGUI();

//=================================================
//  Render Layers
//=================================================

    void RenderLayerToMainPixels(const std::vector<uint32_t>& LayerPixels);
    void SelectLayer(std::vector<uint32_t>*& TargetLayer, int RenderLayer);

    std::vector<uint32_t> PersistentTilesPixels;
    std::vector<uint32_t> TransistentTilesPixels;
    std::vector<uint32_t> PersistentGUIPixels;
    std::vector<uint32_t> TransistentGUIPixels;

    std::vector<uint32_t> RenderedPixels;

//=================================================
//  Text Formating
//=================================================

    void GetFormatedScore(int Score, const std::string& Prefix, std::string& FormatedScoreRef);
    void RewriteFormatedScore(int OldScore, int NewScore, const std::string& Prefix, int TextX, int TextY, int FontSize);

    static constexpr char ValuesAcronyms[5] = {'K', 'M', 'B', 'T', 'Q'};

//=================================================
//  Fonts
//=================================================

    void LoadFontData();
    
    std::unordered_map<char, std::vector<Vector2D<uint8_t>>> Font;

//=================================================
//  GUI Text Position & Font Size Variables
//=================================================

    static constexpr int FirstRoundCounterTopLeftX = 30;
    static constexpr int FirstRoundCounterTopY = 200;
    static constexpr int FirstRoundCounterFontSize = 3;

    static constexpr int AnotherRoundCountersTopLeftX = 30;
    static constexpr int AnotherRoundCountersTopLeftY = 230;
    static constexpr int AnotherRoundCounterFontSize = 2;

    static constexpr int DeadlineValueCounterLeftX = 470;
    static constexpr int DeadlineValueCounterTopY = 30;
    static constexpr int DeadlineValueCounterFontSize = 3;

    static constexpr int CardNameFontSize = 2;
    static constexpr int CardNameInsideOffsetLeftX = 20;
    static constexpr int CardNameInsideOffsetTopY = 20;
    static constexpr int CardTextFontSize = 1;
    static constexpr int CardTextInsideOffsetLeftX = 10;
    static constexpr int CardTextInsideOffsetTopY = 50;

    static constexpr int SpaceBetweenLetters = 2;
    static constexpr int SpaceSize = 2;
    static constexpr int SpaceUnderLetters = 4;
    static constexpr int LetterHeight = 5;
    static constexpr int MaxLetterWidth = 5;

    static constexpr int ScoreCounterLeftX = 30;
    static constexpr int ScoreCounterTopY = 30;
    static constexpr int ScoreFontSize = 4;


//=================================================
//  Flying Text
//=================================================

    std::vector<FloatingText> ActiveTexts;

    static constexpr int ScoreCounterFlyingTextTargetX = ScoreCounterLeftX + 40;
    static constexpr int ScoreCounterFlyingTextTargetY = ScoreCounterTopY + 20;
    static constexpr float ScoreFlyingTextFlightDurationInSec = 60.f;

//=================================================
//  Extra Text Variables
//=================================================

    static inline const std::string ScorePrefix = "";
    static inline const std::string DeadlinePrefix = "NEED ";

};