#include "Render.h"

#include <fstream>

Render::Render()
{
    int AmountOfPixelsOnGL = ConfigSettings::GL_Width_Resolution * ConfigSettings::GL_Height_Resolution;
    PersistentTilesPixels.resize(AmountOfPixelsOnGL);
    std::fill(PersistentTilesPixels.begin(), PersistentTilesPixels.end(), ConfigSettings::Background_Color);

    TransistentTilesPixels.resize(AmountOfPixelsOnGL);
    std::fill(TransistentTilesPixels.begin(), TransistentTilesPixels.end(), ConfigSettings::Empty_Color);

    PersistentGUIPixels.resize(AmountOfPixelsOnGL);
    std::fill(PersistentGUIPixels.begin(), PersistentGUIPixels.end(), ConfigSettings::Empty_Color);

    TransistentGUIPixels.resize(AmountOfPixelsOnGL);
    std::fill(TransistentGUIPixels.begin(), TransistentGUIPixels.end(), ConfigSettings::Empty_Color);

    RenderedPixels.resize(AmountOfPixelsOnGL);

    LoadFontData();
}

void Render::Tick()
{
    UpdateTransistentGUI();

    CombineRenderingLayers();
}

void Render::BeginPlay()
{
}

//=================================================
//  Tick-Based Methods
//=================================================

void Render::CombineRenderingLayers()
{
    int AmountOfPixelsOnGL = ConfigSettings::GL_Width_Resolution * ConfigSettings::GL_Height_Resolution;
    memcpy(RenderedPixels.data(), PersistentTilesPixels.data(), AmountOfPixelsOnGL * sizeof(uint32_t));

    RenderLayerToMainPixels(TransistentTilesPixels);
    RenderLayerToMainPixels(PersistentGUIPixels);
    RenderLayerToMainPixels(TransistentGUIPixels);
}

//Animate next frame of flying text.
void Render::UpdateTransistentGUI()
{
    //Erase old pixels in layer.
    std::fill(TransistentGUIPixels.begin(), TransistentGUIPixels.end(), ConfigSettings::Empty_Color);

    for (auto FloatingTextInstance = ActiveTexts.begin(); FloatingTextInstance != ActiveTexts.end();) 
    {
        FloatingTextInstance->PositionX += FloatingTextInstance->SpeedX;
        FloatingTextInstance->PositionY += FloatingTextInstance->SpeedY;
        FloatingTextInstance->LifeTime--;

        WriteSentence(FloatingTextInstance->PositionX, FloatingTextInstance->PositionY, FloatingTextInstance->Text, FloatingTextInstance->FontSize, 3);

        if (FloatingTextInstance->LifeTime <= 0)
        {
            ActiveTexts.erase(FloatingTextInstance);
        }
        else
        {
            FloatingTextInstance++;
        }
    }
}

//=================================================
//  Render Layers
//=================================================

void Render::RenderLayerToMainPixels(const std::vector<uint32_t>& LayerPixels)
{
    int AmountOfPixelsOnGL = ConfigSettings::GL_Width_Resolution * ConfigSettings::GL_Height_Resolution;
    for (int i = 0; i < AmountOfPixelsOnGL; ++i)
    {
        uint32_t Pixel = LayerPixels[i];
        uint8_t Alpha = Pixel >> 24;
        if (Alpha != 0) {
            RenderedPixels[i] = Pixel;
        }
    }
}

void Render::SelectLayer(std::vector<uint32_t>*& TargetLayer, int RenderLayer)
{
    switch(RenderLayer) {
        case 0:
            TargetLayer = &PersistentTilesPixels;
            break;
        case 1:
            TargetLayer = &TransistentTilesPixels;
            break;
        case 2:
            TargetLayer = &PersistentGUIPixels;
            break;
        case 3:
            TargetLayer = &TransistentGUIPixels;
            break;
        default:
            TargetLayer = &PersistentGUIPixels;
    }
}

//=================================================
//  Request Draw
//================================================= 

void Render::RequestTileDraw(int X, int Y, bool IsFilled, int Color, int RenderLayer)
{
    std::vector<uint32_t>* TargetLayer = nullptr;

    SelectLayer(TargetLayer, RenderLayer);

    TileDrawInstance.DrawTile(X, Y, IsFilled, Color, TargetLayer);
}

void Render::RequestDrawTileSprite(int X, int Y, int TileIndex, int SpriteID){
    int TilesHeight = TilesParameters::TilesHeight;

    //Erase whole tile.
    RequestTileDraw(X, Y - (TilesHeight/2 + 1), true, ConfigSettings::Background_Color, RenderLayer::PersistentTiles);

    //Draw empty tile.
    RequestTileDraw(X, Y - (TilesHeight/2 + 1), false, ConfigSettings::Definition_Color, RenderLayer::PersistentTiles);

    TileDrawInstance.DrawSpriteToTile(X, Y, TileIndex, SpriteID, PersistentTilesPixels);
}

//=================================================
//  Request Generic Text
//=================================================  

void Render::RequestTileScoreText(int StartX, int StartY, int ScoreIn)
{
    std::string ScoreText = std::to_string(ScoreIn);

    RequestFlyingText(StartX, StartY, ScoreText, ScoreCounterFlyingTextTargetX, ScoreCounterFlyingTextTargetY, ScoreFlyingTextFlightDurationInSec, 2);
}

void Render::RequestRewriteScore(int OldScore, int NewScore)
{
    RewriteFormatedScore(OldScore, NewScore, ScorePrefix, ScoreCounterLeftX, ScoreCounterTopY, ScoreFontSize);
}

void Render::RequestFlyingText(int StartX, int StartY, std::string Text, int TargetX, int TargetY, float TravelTime, int FontSize)
{
    FloatingText FloatingTextInstance;

    FloatingTextInstance.Text = Text;
    FloatingTextInstance.PositionX = static_cast<float>(StartX);
    FloatingTextInstance.PositionY = static_cast<float>(StartY);
    FloatingTextInstance.TargetX = static_cast<float>(TargetX);
    FloatingTextInstance.TargetY = static_cast<float>(TargetY);

    float DistanceX = TargetX - StartX;
    float DistanceY = TargetY - StartY;
    FloatingTextInstance.SpeedX = DistanceX / TravelTime;
    FloatingTextInstance.SpeedY = DistanceY / TravelTime;

    FloatingTextInstance.LifeTime = TravelTime;
    FloatingTextInstance.FontSize = FontSize;
    ActiveTexts.push_back(FloatingTextInstance);
}

//=================================================
//  Request Counters Text
//================================================= 

void Render::RequestClicksAmountLeftText(int ClicksRemainingBefore, int ClicksRemainingNow)
{
    std::string AmountOfClicksBeforeText = std::to_string(ClicksRemainingBefore);
    EraseText(FirstRoundCounterTopLeftX, FirstRoundCounterTopY, AmountOfClicksBeforeText, FirstRoundCounterFontSize, RenderLayer::PersistentGUI);

    std::string AmountOfClicksNowText = std::to_string(ClicksRemainingNow);
    WriteSentence(FirstRoundCounterTopLeftX, FirstRoundCounterTopY, AmountOfClicksNowText, FirstRoundCounterFontSize, RenderLayer::PersistentGUI);
}

void Render::RequestErasePrimaryClickCounter(int ClicksRemainingBefore)
{
    EraseText(
        FirstRoundCounterTopLeftX, 
        FirstRoundCounterTopY, 
        std::to_string(ClicksRemainingBefore), 
        FirstRoundCounterFontSize, 
        RenderLayer::PersistentGUI
    );
}

void Render::RequestEraseSecondaryClickCounter(int ClicksRemainingBefore, int CounterIndex)
{
    int RelativeX = AnotherRoundCountersTopLeftX;
    int RelativeY = AnotherRoundCountersTopLeftY + CounterIndex * (AnotherRoundCounterFontSize * (LetterHeight + SpaceUnderLetters));

    EraseText(RelativeX, RelativeY, std::to_string(ClicksRemainingBefore), AnotherRoundCounterFontSize, AnotherRoundCounterFontSize);
}

void Render::RequestRewriteDeadlineScore(int OldScore, int NewScore)
{
    RewriteFormatedScore(OldScore, NewScore, DeadlinePrefix, DeadlineValueCounterLeftX, DeadlineValueCounterTopY, DeadlineValueCounterFontSize);
}

//=================================================
//  Draw & Erase Draw
//================================================= 

void Render::DrawCard(int X, int Y, std::string CardName, std::string CardText)
{
    for(int SpaceX = 0; SpaceX < PositionSpecialGUI::CardWidth; SpaceX++)
    {
        int TopSideIndex = (X + SpaceX) + (Y * ConfigSettings::GL_Width_Resolution);
        int BottomSideIndex = (X + SpaceX) + ((Y + PositionSpecialGUI::CardHeight) * ConfigSettings::GL_Width_Resolution);
        PersistentGUIPixels[TopSideIndex] = ConfigSettings::Definition_Color;
        PersistentGUIPixels[BottomSideIndex] = ConfigSettings::Definition_Color;
    }

    for(int SpaceY = 0; SpaceY < PositionSpecialGUI::CardHeight; SpaceY++)
    {
        int LeftSideIndex = (X) + ((Y + SpaceY) * ConfigSettings::GL_Width_Resolution);
        int RightSideIndex = (X + PositionSpecialGUI::CardWidth) + ((Y + SpaceY) * ConfigSettings::GL_Width_Resolution);
        PersistentGUIPixels[LeftSideIndex] = ConfigSettings::Definition_Color;
        PersistentGUIPixels[RightSideIndex] = ConfigSettings::Definition_Color;
    }

    WriteSentence(X + CardNameInsideOffsetLeftX, Y + CardNameInsideOffsetTopY, CardName, CardNameFontSize, RenderLayer::PersistentGUI);
    WriteSentence(X + CardTextInsideOffsetLeftX, Y + CardTextInsideOffsetTopY, CardText, CardTextFontSize, RenderLayer::PersistentGUI);
}

void Render::EraseSpace(int X, int Y, int Width, int Height, int RenderLayer)
{
    uint32_t Color = ConfigSettings::Empty_Color;

    std::vector<uint32_t>* TargetLayer = nullptr;

    SelectLayer(TargetLayer, RenderLayer);

    for(int SpaceX = 0; SpaceX < Width; SpaceX++)
    {
        for(int SpaceY = 0; SpaceY < Height; SpaceY++)
        {
            (*TargetLayer)[(X + SpaceX) + (Y + SpaceY) * ConfigSettings::GL_Width_Resolution] = Color;
        }
    }
}

//=================================================
//  Text & Erase Text
//================================================= 

void Render::EraseText(int X, int Y, std::string Text, int FontSize, int RenderLayer)
{
    int WidthOfText = 0;
    int HeightOfText = LetterHeight * FontSize;
    int SideOffset = 0;
    int TopOffset = 0;
    int TextLen = Text.size();
    for(int i = 0; i < TextLen; i++)
    {
        if(Text[i] == '\n')
        {
            if(SideOffset > WidthOfText)
                WidthOfText = SideOffset;
            SideOffset = 0;
            TopOffset += ((LetterHeight + SpaceUnderLetters)  * FontSize);
            continue;
        }
        if(Text[i] == ' ')
        {
            SideOffset += SpaceSize * FontSize;
            continue;
        }

        SideOffset += SpaceBetweenLetters + Font.find(Text[i])->second[0].X * FontSize;
        if(SideOffset > WidthOfText)
            WidthOfText = SideOffset;

        HeightOfText += TopOffset;
    }

    EraseSpace(X, Y, WidthOfText, HeightOfText, RenderLayer);

}

void Render::DrawLetter(int X, int Y, std::vector<Vector2D<uint8_t>> LetterBitmap, int FontSize, int RenderLayer) 
{
    std::vector<uint32_t>* TargetLayer = nullptr;

    SelectLayer(TargetLayer, RenderLayer);

    for (size_t i = 1; i < LetterBitmap.size(); ++i)
    {
        for(int SpaceX = 0; SpaceX < FontSize; SpaceX++)
        {
            for(int SpaceY = 0; SpaceY < FontSize; SpaceY++)
            {
                int RelativeX = (LetterBitmap[i].X * FontSize) + SpaceX;
                int RelativeY = (LetterBitmap[i].Y * FontSize) + SpaceY;
                (*TargetLayer)[(X + RelativeX) + (Y + RelativeY) * ConfigSettings::GL_Width_Resolution] = ConfigSettings::Definition_Color;
            }
        }
    }
}

void Render::WriteSentence(int X, int Y, std::string Text, int FontSize, int RenderLayer)
{
    int SideOffset = 0;
    int TopOffset = 0;
    int TextLen = Text.size();
    for(int i = 0; i < TextLen; i++)
    {
        if(Text[i] == '\n')
        {
            SideOffset = 0;
            TopOffset += ((LetterHeight + SpaceUnderLetters) * FontSize);
            continue;
        }
        if(Text[i] == ' ')
        {
            SideOffset += SpaceSize * FontSize;
            continue;
        }

        DrawLetter(X + SideOffset, Y + TopOffset, Font.find(Text[i])->second, FontSize, RenderLayer);
        SideOffset += SpaceBetweenLetters + Font.find(Text[i])->second[0].X * FontSize;
    }
}

//=================================================
//  Text Formating
//=================================================

void Render::GetFormatedScore(int Score, const std::string& Prefix, std::string& FormatedScoreRef)
{
    double DisplayScore = static_cast<double>(Score);
    int Thousands = 0;
    while(DisplayScore > 1000)
    {
        DisplayScore /= 1000;
        Thousands++;
    }

    //Avoid decimal numbers for score 0.
    char Buffer[32];
    if(Score == 0)
    {
        std::snprintf(Buffer, sizeof(Buffer), "%.0f", DisplayScore);
    } 
    else
    {
        std::snprintf(Buffer, sizeof(Buffer), "%.2f", DisplayScore);
    }

    FormatedScoreRef = Prefix;
    FormatedScoreRef += Buffer;

    //Convert number to acronym, like 1000 = 1K
    if(Thousands > 0)
        FormatedScoreRef += ValuesAcronyms[Thousands - 1];
}

void Render::RewriteFormatedScore(int OldScore, int NewScore, const std::string& Prefix, int TextX, int TextY, int FontSize)
{
    std::string FormatedScore;

    GetFormatedScore(OldScore, Prefix, FormatedScore);

    EraseText(TextX, TextY, FormatedScore, FontSize, RenderLayer::PersistentGUI);

    GetFormatedScore(NewScore, Prefix, FormatedScore);

    WriteSentence(TextX, TextY, FormatedScore, FontSize, RenderLayer::PersistentGUI);
}

//=================================================
//  Counters Text
//=================================================  

void Render::DrawNewRoundsCounters(int ClicksPerRound, int RoundsPerDeathLine)
{
    if(RoundsPerDeathLine > 0)
    {
        std::string Amount = std::to_string(ClicksPerRound);
        WriteSentence(FirstRoundCounterTopLeftX, FirstRoundCounterTopY, Amount, FirstRoundCounterFontSize, RenderLayer::PersistentGUI);
    }

    for(int i = 0; i < RoundsPerDeathLine - 1; i++)
    {
        std::string Amount = std::to_string(ClicksPerRound);
        WriteSentence
        (
            AnotherRoundCountersTopLeftX, 
            AnotherRoundCountersTopLeftY + (i * (LetterHeight + SpaceUnderLetters) * AnotherRoundCounterFontSize), 
            Amount, 
            AnotherRoundCounterFontSize, 
            RenderLayer::PersistentGUI
        );
    }
}

//=================================================
//  Fonts
//=================================================

void Render::LoadFontData()
{
    std::ifstream File("assets/font.bin", std::ios::binary);
    if (!File)
    {
        printf("Failed to open font binary.");
        return;
    }

    while (File)
    {
        uint8_t AmountOfDataInLetter;
        File.read(reinterpret_cast<char*>(&AmountOfDataInLetter), 1);
        if(!File)
            break;

        char CharacterOfFont;
        File.read(reinterpret_cast<char*>(&CharacterOfFont), 1);

        uint8_t WidthOfFontCharacter;
        File.read(reinterpret_cast<char*>(&WidthOfFontCharacter), 1);

        std::vector<Vector2D<uint8_t>> FontCharacterPixels;
        FontCharacterPixels.reserve(AmountOfDataInLetter - 1);

        FontCharacterPixels.push_back(Vector2D<uint8_t>{ WidthOfFontCharacter, 0 });

        for(uint8_t i = 0; i < AmountOfDataInLetter - 2; i++)
        {
            uint8_t PixelIndex;
            File.read(reinterpret_cast<char*>(&PixelIndex), 1);

            uint8_t X = (PixelIndex % MaxLetterWidth);
            uint8_t Y = (PixelIndex / MaxLetterWidth);
            FontCharacterPixels.emplace_back(Vector2D<uint8_t>{ X, Y });
        }

        Font.emplace(CharacterOfFont, std::move(FontCharacterPixels));
    }

}

//=================================================
//  Getters
//=================================================

std::vector<uint32_t>* Render::GetRenderedPixelsRef()
{
    return &RenderedPixels;
}