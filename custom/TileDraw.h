#pragma once

#include "../src/Definitions.h"

#include <vector>

class Render;

class TileDraw
{

public:
    TileDraw();

//=================================================
//  Draw Tile Or Sprite
//=================================================

    void DrawTile(int X, int Y, bool IsFilled, int color, std::vector<uint32_t>* TargetLayer);
    void DrawSpriteToTile(int X, int Y, int TileIndex, int SpriteIndex, std::vector<uint32_t>& TargetLayer);

private:

//=================================================
//  Sprites Private
//=================================================

    void LoadSpriteData();

    std::vector<std::vector<Vector2D<uint8_t>>> Sprites;

    static constexpr int SpriteDimension = 16;
    static constexpr int SpriteCoordinationOffset = 8;

//=================================================
//  Tile Variables
//=================================================

    static constexpr int TilesTopCornerX = ConfigSettings::GL_Width_Resolution/2;
    static constexpr int TilesTopCornerY = 50;
    static constexpr int GapBetweenTiles = 3;
};