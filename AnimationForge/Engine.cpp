#include "Engine.h"

Engine::Engine()
	:
	wnd
	(
		Animation4ge::WindowWidth, Animation4ge::WindowHeight, "Animation Forge",
		{ Animation4ge::BackgroundLayerRes,Animation4ge::MainLayerRes,Animation4ge::TextLayerRes,Animation4ge::UILayerRes }
	),
	kbd(wnd.kbd),
	mouse(wnd.mouse),
	gfx(wnd.gfx()),
	AppClock()
{

}

void Engine::Go()
{
	gfx.NewFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Engine::UpdateModel()
{

}

void Engine::ComposeFrame()
{

}
