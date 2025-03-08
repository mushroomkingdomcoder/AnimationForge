#pragma once
#include "Clock.h"
#include "Window.h"
#include "Animation.h"
#include "UserInterface.h"

class Animation4ge
{
public:
	static constexpr int WindowWidth				= 1280;
	static constexpr int WindowHeight				= 720;
	static constexpr int2 BackgroundLayerRes		= { 1280,720 };
	static constexpr int2 MainLayerRes				= { 1280,720 };
	static constexpr int2 TextLayerRes				= { 1280,720 };
	static constexpr int2 UILayerRes				= { 1280,720 };
private:
	static constexpr int BackgroundLayer			= 0;
	static constexpr int MainLayer					= 1;
	static constexpr int TextLayer					= 2;
	static constexpr int UILayer					= 3;
	static constexpr int nUIObj						= 9;
	static constexpr Color BackgroundColor			= Colors::Black;
	static constexpr vec2i MaxAnimationFrameDim		= { 500,300 };
	static constexpr int MaxFramesPerAnimation		= 128;
	static constexpr int MaxAnimations				= 32;
	static constexpr vec2i FrameSelectionWindowDim  = { 1000,600 };
	static constexpr vec2i FrameZoom				= FrameSelectionWindowDim / MaxAnimationFrameDim;
	static constexpr float DefaultFPS				= 15.0f;
	static constexpr float MinFPS					= 0.5f;
	static constexpr float MaxFPS					= 90.0f;
	static constexpr float StepFPS					= 1.25f;
	static constexpr Color FrameSelectorColor		= Colors::BrightBlue;
private:
	enum class UIs
	{
		ViewAnimBtn, ImportAnimBtn, BackBtn, PlayPause, Slow, Fast, Prev, Next, Zoom
	};
private:
	Clock clock;
	Window& wnd;
	Mouse& mouse;
	Keyboard& kbd;
	Graphics& gfx;
	GraphicText gfcText;
private:
	std::vector<Animation> animations;
	enum class State
	{
		MainMenu, ViewAnimations, ImportAnimation
	} state;
	bool stateChange;
private:
	void ResetDisplay();

	void MainMenu();
	struct UIButtonData
	{
		struct Refs
		{
			Mouse& mouse;
			Animation4ge::State& appState;
			bool& appStateChange;
			const Window& wnd;
			bool& paused;
			bool& zoomed;
			int& nAnimation;
			Animation** ppCurAnimation;
			const std::vector<Animation>& animations;
		} refs;
		bool hovering[nUIObj];
	} bdata;
	std::unique_ptr<UserInterface::Object> pViewAnimationsBtn;
	std::unique_ptr<UserInterface::Object> pImportAnimationBtn;
	std::unique_ptr<UserInterface::Object> pBackButton;
	std::unique_ptr<UserInterface::Object> pPlayPause;
	std::unique_ptr<UserInterface::Object> pSlow;
	std::unique_ptr<UserInterface::Object> pFast;
	std::unique_ptr<UserInterface::Object> pPrev;
	std::unique_ptr<UserInterface::Object> pNext;
	std::unique_ptr<UserInterface::Object> pZoom;
	UserInterface mainMenuInterface;

	void ImportAnimation();
	std::vector<std::wstring> wfiles;
	int animImportIndex;
	bool newAnim;
	bool skipAnim;
	Image curImportImg;
	int2 imgPos;
	int2 frameDim;
	iRect sizeSelector;
	iRect imgRect;
	int2 mousePos;

	void ViewAnimations();
	int nAnimation = 0;
	bool paused = false;
	bool zoomed = true;
	Animation* pCurAnimation = nullptr;

public:
	Animation4ge(Window& wnd);
	void Go();
};
