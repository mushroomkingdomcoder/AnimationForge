#pragma once
#include "Clock.h"
#include "Window.h"
#include "Animation.h"
#include "UserInterface.h"

class Animation4ge
{
public:
	static constexpr int WindowWidth			= 800;
	static constexpr int WindowHeight			= 600;
	static constexpr int2 BackgroundLayerRes	= { 800,600 };
	static constexpr int2 MainLayerRes			= { 800,600 };
	static constexpr int2 TextLayerRes			= { 800,600 };
	static constexpr int2 UILayerRes			= { 800,600 };
	static constexpr int BackgroundLayer		= 0;
	static constexpr int MainLayer				= 1;
	static constexpr int TextLayer				= 2;
	static constexpr int UILayer				= 3;
	static constexpr Color BackgroundColor = Colors::Black;
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
		MainMenu, NewAnimation, ImportAnimation, ExportAnimation, EditAnimation, EditImage
	} state;
	bool stateChange;
private:
	void MainMenu();
	struct MenuButtonData
	{
		bool mouseIsHovering;
		Mouse& mouse;
		State& appState;
		bool& appStateChange;
	};
	std::vector<MenuButtonData> mbd;
	std::unique_ptr<UserInterface::Object> pNewAnimationBtn;
	std::unique_ptr<UserInterface::Object> pImportAnimationBtn;
	std::unique_ptr<UserInterface::Object> pExportAnimationBtn;
	std::unique_ptr<UserInterface::Object> pEditAnimationBtn;
	UserInterface mainMenuInterface;
public:
	Animation4ge(Window& wnd);
	void Go();
};

