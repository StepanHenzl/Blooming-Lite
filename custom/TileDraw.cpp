#include "TileDraw.h"

#include <fstream>

#include "../src/UserInstance.h"

TileDraw::TileDraw()
{
    LoadSpriteData();
}

//=================================================
//  Draw Tile Or Into Tile
//=================================================

void TileDraw::DrawTile(int X, int Y, bool IsFilled, int color, std::vector<uint32_t>* TargetLayer)
{
    int TilesHeight = TilesParameters::TilesHeight;
    int HalfHeight = TilesHeight / 2;
    for(int i = 0; i < TilesHeight; i++)
    {
        int RelativeY = Y + i;
        int RelativePositiveX;
        int RelativeNegativeX;
        int RowLength;

        if(i <= HalfHeight)
        {
            RelativePositiveX = X + i;
            RelativeNegativeX = X - i;
            RowLength = i;
        }
        else
        {
            RelativePositiveX = X + ((TilesHeight - 1) - i);
            RelativeNegativeX = X - ((TilesHeight - 1) - i);
            RowLength = (TilesHeight - 1) - i;
        }

        if(IsFilled)
        {
            for(int j = 0; j < RowLength; j++)
            {
                (*TargetLayer)[RelativeNegativeX + RelativeY * ConfigSettings::GL_Width_Resolution + j + 1] = color;
                (*TargetLayer)[RelativePositiveX + RelativeY * ConfigSettings::GL_Width_Resolution - j - 1] = color;
            }
        } 
        else
        {
            (*TargetLayer)[RelativePositiveX + RelativeY * ConfigSettings::GL_Width_Resolution] = color;
            (*TargetLayer)[RelativeNegativeX + RelativeY * ConfigSettings::GL_Width_Resolution] = color;
        }
    }
}

void TileDraw::DrawSpriteToTile(int X, int Y, int TileIndex, int SpriteIndex, std::vector<uint32_t>& TargetLayer)
{
    for(size_t i = 0; i < Sprites[SpriteIndex].size(); i++)
    {
        int RelativeX = static_cast<int>(Sprites[SpriteIndex][i].X) - SpriteCoordinationOffset;
        int RelativeY = static_cast<int>(Sprites[SpriteIndex][i].Y) - SpriteCoordinationOffset;
        TargetLayer[(X + RelativeX) + (Y + RelativeY) * ConfigSettings::GL_Width_Resolution] = ConfigSettings::Definition_Color;
    }
}

//=================================================
//  Sprites Private
//=================================================

void TileDraw::LoadSpriteData()
{
    std::ifstream File("assets/sprites.bin", std::ios::binary);
    if (!File)
    {
        printf("Failed to open sprites binary.");
        return;
    }

    while (File)
    {
        uint8_t AmountOfPixelsInSprite;
        File.read(reinterpret_cast<char*>(&AmountOfPixelsInSprite), 1);
        if(!File)
            break;

        std::vector<Vector2D<uint8_t>> Sprite;
        Sprite.reserve(AmountOfPixelsInSprite);

        for(uint8_t i = 0; i < AmountOfPixelsInSprite; i++)
        {
            uint8_t PixelIndex;
            File.read(reinterpret_cast<char*>(&PixelIndex), 1);

            //Each pixel is stored as a single byte index in a 16×16 sprite grid. Convert the index to coordinates.
            uint8_t X = (PixelIndex % SpriteDimension);
            uint8_t Y = (PixelIndex / SpriteDimension);
            Sprite.emplace_back(Vector2D<uint8_t>{ X, Y });
        }

        Sprites.push_back(std::move(Sprite));
    }

    //Insert empty sprite for empty tile
    std::vector<Vector2D<uint8_t>> EmptySprite;
    auto Position = Sprites.begin() + PlantLevel::EmptyTile;
    Sprites.insert(Position, EmptySprite);
}