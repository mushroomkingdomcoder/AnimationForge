#pragma once
#include "Clock.h"
#include "Window.h"
#include "Animation.h"

class Animation4ge
{
public:
	static constexpr int WindowWidth			= 800;
	static constexpr int WindowHeight			= 600;
	static constexpr int2 BackgroundLayerRes	= { 1024,576  };
	static constexpr int2 MainLayerRes			= { 1024,576  };
	static constexpr int2 TextLayerRes			= { 512 ,288  };
	static constexpr int2 UILayerRes			= { 1024,576  };
	static constexpr int BackgroundLayer		= 0;
	static constexpr int MainLayer				= 1;
	static constexpr int TextLayer				= 2;
	static constexpr int UILayer				= 3;
private:
	Clock clock;
	Window& wnd;
	Mouse& mouse;
	Keyboard& kbd;
	Graphics& gfx;
private:
	std::vector<Animation> animations;
public:
	enum class State
	{
		MainMenu, EditImage, EditAnimation, AnimationViewer
	};
	State state;
public:
	Animation4ge(Window& wnd, Graphics& gfx);
	void MainMenu();
	void EditImage();
	void Editanimation();
	void AnimationViewer();
};
