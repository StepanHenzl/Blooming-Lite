#pragma once

#include <string>

#include <emscripten/emscripten.h>

namespace Sounds
{

    enum SoundIndexes
    {
        TileHoverSound,
        BarrenLeftClickSound,
        LevelUp,
        Score1,
        Score2,
        Score3,
        NUM
    };

    extern std::string SoundFiles[NUM];

    void RequestSound(int SoundIndex);

}