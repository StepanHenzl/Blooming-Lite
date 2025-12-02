#pragma once

#include <stdint.h>
#include <string>

namespace ConfigSettings
{
    constexpr int GL_Width_Resolution = 640;
    constexpr int GL_Height_Resolution = 360;

    constexpr int32_t Empty_Color = 0x00000000;
    constexpr int32_t Background_Color = 0xFF000000;
    constexpr int32_t Definition_Color = 0xFFFFFFFF;
}

namespace PlantLevel
{
    constexpr int Barren = 0;
    constexpr int EmptyTile = 1;
    constexpr int ShortGrass = 2;
    constexpr int MediumGrass = 3;
    constexpr int LongGrass = 4;
    constexpr int SmallFlower = 5;
    constexpr int MediumFlower = 6;
    constexpr int BigFlower = 7;
    constexpr int Mushroom = 8;
    constexpr int SmallTree = 9;
    constexpr int MediumTree = 10;
    constexpr int BigTree = 11;
};

namespace RenderLayer
{
    constexpr int PersistentTiles = 0;
    constexpr int TransistentTiles = 1;
    constexpr int PersistentGUI = 2;
    constexpr int TransistentGUI = 3;
}

namespace TilesParameters
{
    constexpr int TilesHeight = 23;
}

namespace PositionSpecialGUI
{
    constexpr int PauseTextLeftX = 200;
	constexpr int PauseTextTopY = 300;

    constexpr int CardsPositionLeftX[3] = {50, 250, 450};
    constexpr int CardPositionTopY = 70;
    constexpr int CardHeight = 200;
    constexpr int CardWidth = 150;

    constexpr int PositionLoseText1X = 200;
    constexpr int PositionLoseText1Y = 100;

    constexpr int PositionLoseText2X = 200;
    constexpr int PositionLoseText2Y = 220;
}

namespace TextSpecialGUI
{
    const std::string ClickToContinueText = "CLICK TO CONTINUE";
    constexpr int ClickToContinueFontSize = 2;

    const std::string LoseText1 = "YOU LOSE";
    constexpr int LoseTextFontSize1 = 8;

    const std::string LoseText2 = "CLICK TO CONTINUE";
    constexpr int LoseTextFontSize2 = 4;
}

template<typename Type>
struct Vector2D
{
    Type X;
    Type Y;
};