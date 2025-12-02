#pragma once

#include "../src/Definitions.h"

class Init;

class UserInstance;

class MouseMovement
{
public:
    MouseMovement(Init& InInitRef);

    void BeginPlay();

//=================================================
//  Mouse Movement
//=================================================

    void CheckMouseMovement();

//=================================================
//  Mouse Actions
//=================================================

    void LeftMouseButtonPressed();
    void LeftMouseButtonReleased();


    void RequestLeftMouseButtonUnpowered();

private:

//=================================================
//  Instances & References
//=================================================

    Init& InitRef;

//=================================================
//  Mouse Actions Variables
//=================================================   

    bool IsLeftMouseButtonHold = false;
};