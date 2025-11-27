#pragma once

#include <bitset>
#include <random>

#include "Definitions.h"

#include "InputHandler.h"
#include "../custom/Init.h"

class Init;

#ifdef __cplusplus
extern "C" 
{
#endif
	//Initiate render.
	EMSCRIPTEN_KEEPALIVE void js_init_gl(int w, int h);
	//Render frame - need camera pixels ready.
	EMSCRIPTEN_KEEPALIVE void js_render_frame();
	//Get the distance of the canvas from left top corner, used to calculate start coordinates of canvas.
	EMSCRIPTEN_KEEPALIVE void js_get_canvas_distance(double* outX, double* outY);
	//Get the actual screen size for WebGL ratio calculations.
	EMSCRIPTEN_KEEPALIVE void js_get_canvas_size(double* widthPtr, double* heightPtr);

#ifdef __cplusplus
}
#endif

class UserInstance
{
public:
	static UserInstance& UserInstanceGet()
	{
		static UserInstance UserInstanceRef;
		return UserInstanceRef;
	}
	//Engine tick. Handles non-fps based operations like rendering.
	void Tick();
	//Engine logical tick. Handles fps based operations that need delta time like physics.
	void LogicalTick();

	void BeginPlay();


//=================================================
//  Rendering
//=================================================

	std::vector<uint32_t>* GetRenderedPixelsRef();

//=================================================
//  Audio
//=================================================

	void PlaySound(const char* Path);
	float GetVolume();

//=================================================
//  Randomness
//=================================================

	std::mt19937 RandomnessGenerator;

//=================================================
//  Inputs
//=================================================

	void RequestRegisterMouseUpSubscriber(std::function<void()> FuncCallback);
	void RequestRegisterMouseDownSubscriber(std::function<void()> FuncCallback);

	//Array that contain ascii values of all pressed keys.
	std::bitset<256>* GetPressedKeyListRef();


private:
	UserInstance();
	~UserInstance();
	UserInstance(const UserInstance&) = delete;
	UserInstance& operator=(const UserInstance&) = delete;

//=================================================
//  Instances & References
//=================================================

	Init InitInstance;
	InputHandler InputHandlerInstance;

//=================================================
//  Rendering Private
//=================================================

	void AskForRenderedPixels();
	std::vector<uint32_t>* RenderedPixels;

	std::bitset<256> PressedKeyList;

//=================================================
//  Tick Variables
//=================================================
	//When last frame happend.
	double LastFrameTime = 0.0;
	//Diff between last two frames.
	double DeltaAccumulation = 0.0;
	//FPS count for logical tick.
	static constexpr double LogicalTickLengthInMS = 1000.0 / 60.0;

//=================================================
//  Audio Private
//=================================================

	float Volume = 0.1;

};
