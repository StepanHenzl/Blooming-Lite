#include "Sounds.h"

#include "../src/UserInstance.h"

std::string Sounds::SoundFiles[NUM] = 
{
    "CSS/TileHoverSound.wav",
    "CSS/BarrenLeftClickSound.wav",
    "CSS/LevelUp.wav",
    "CSS/GetScore1.wav",
    "CSS/GetScore2.wav",
    "CSS/GetScore3.wav"
};

void Sounds::RequestSound(int SoundIndex)
{
    if(SoundIndex >= NUM || SoundIndex < 0)
    {
        return;
    }

    UserInstance::UserInstanceGet().PlaySound(SoundFiles[SoundIndex].c_str());
}
