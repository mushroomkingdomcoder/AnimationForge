#include "Animation4ge.h"

using UI = UserInterface;
using UIGFCA = UI::Graphic<Animation>;
using UIGFCI = UI::Graphic<Image>;

void Animation4ge::MainMenu()
{
	switch (state)
	{
		case State::MainMenu:
		{
			float time = clock.Mark();
			if (stateChange)
			{
				gfx.Erase(TextLayer);
				gfx.Erase(UILayer);
				Image("graphics\\banner-800x100.bmp").Draw(gfx, 0, 0, UILayer);
				stateChange = false;
			}
			mainMenuInterface.Update(time);
			mainMenuInterface.Draw();
		}
		case State::ImportAnimation:
		{

		}
		case State::ExportAnimation:
		{

		}
	}
}

Animation4ge::Animation4ge(Window& wnd)
	:
	clock(),
	wnd(wnd),
	mouse(wnd.mouse),
	kbd(wnd.kbd),
	gfx(wnd.gfx()),
	gfcText(gfx.GetPixelMap(TextLayer).data(), (vec2i(TextLayerRes) / 16).GetVStruct()),
	state(State::MainMenu),
	stateChange(true),
	mbd({ { false,mouse,state,stateChange },{ false,mouse,state,stateChange },{ false,mouse,state,stateChange },{ false,mouse,state,stateChange } }),
	pNewAnimationBtn
	(	std::make_unique<UIGFCA>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - (10 + 100 + 20 + 100), Animation("graphics\\new-anim-btn-1200x100-4x1.bmp", { 300,100 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			MenuButtonData& mbd = reinterpret_cast<MenuButtonData&>(*thisGfc.pData);
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (iRect({ thisGfc.x,thisGfc.y }, { 300,100 }).ContainsPoint({ mbd.mouse.GetX(),mbd.mouse.GetY() }))
			{
				if (!mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
						update = true;
					}
				}
				if (mbd.mouse.LeftIsClicked())
				{
					mbd.appState = State::NewAnimation;
					mbd.appStateChange = true;
				}
			}
			else
			{
				if (mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
						update = true;
					}
				}
			}
			return update;
		}, 
		reinterpret_cast<char*>(&mbd[0]))
	),
	pImportAnimationBtn
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - (10 + 100), Image("graphics\\import-anim-btn-300x100.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			MenuButtonData& mbd = reinterpret_cast<MenuButtonData&>(*thisGfc.pData);
			if (iRect({ thisGfc.x,thisGfc.y }, { 300,100 }).ContainsPoint({ mbd.mouse.GetX(),mbd.mouse.GetY() }))
			{
				if (!mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = true;
					thisGfc.graphic.Rotate180();
					return true;
				}
				if (mbd.mouse.LeftIsClicked())
				{
					mbd.appState = State::ImportAnimation;
					mbd.appStateChange = true;
				}
			}
			else
			{
				if (mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = false;
					thisGfc.graphic.Rotate180();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&mbd[1]))
	),
	pExportAnimationBtn
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 + (10), Image("graphics\\export-anim-btn-300x100.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			MenuButtonData& mbd = reinterpret_cast<MenuButtonData&>(*thisGfc.pData);
			if (iRect({ thisGfc.x,thisGfc.y }, { 300,100 }).ContainsPoint({ mbd.mouse.GetX(),mbd.mouse.GetY() }))
			{
				if (!mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = true;
					thisGfc.graphic.Rotate180();
					return true;
				}
				if (mbd.mouse.LeftIsClicked())
				{
					mbd.appState = State::ExportAnimation;
					mbd.appStateChange = true;
				}
			}
			else
			{
				if (mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = false;
					thisGfc.graphic.Rotate180();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&mbd[2]))
	),
	pEditAnimationBtn
	(	std::make_unique<UIGFCA>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 + (10 + 100 + 20), Animation("graphics\\edit-anim-btn-1200x100-4x1.bmp", { 300,100 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			MenuButtonData& mbd = reinterpret_cast<MenuButtonData&>(*thisGfc.pData);
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (iRect({ thisGfc.x,thisGfc.y }, { 300,100 }).ContainsPoint({ mbd.mouse.GetX(),mbd.mouse.GetY() }))
			{
				if (!mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
						update = true;
					}
				}
				if (mbd.mouse.LeftIsClicked())
				{
					mbd.appState = State::EditAnimation;
					mbd.appStateChange = true;
				}
			}
			else
			{
				if (mbd.mouseIsHovering)
				{
					mbd.mouseIsHovering = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
						update = true;
					}
				}
			}
			return update;
		}, 
		reinterpret_cast<char*>(&mbd[3]))
	),
	mainMenuInterface(gfx, "charsets\\default.bmp", { 16,6 }, ' ', UILayer)
{
	mainMenuInterface.AddInterface(pNewAnimationBtn);
	mainMenuInterface.AddInterface(pImportAnimationBtn);
	mainMenuInterface.AddInterface(pExportAnimationBtn);
	mainMenuInterface.AddInterface(pEditAnimationBtn);
	gfx.SetBackgroundColor(BackgroundColor);
	gfx.ManuallyManage(TextLayer);
	gfx.ManuallyManage(UILayer);
	clock.Mark();
}

void Animation4ge::Go()
{
	switch (state)
	{
	case State::MainMenu:
	case State::ImportAnimation:
	case State::ExportAnimation:
		MainMenu();
		break;
	case State::NewAnimation:
	case State::EditAnimation:
	case State::EditImage:
	default:
		throw EXCPT_NOTE("Unknown runtime error occurred!");
	}
}

