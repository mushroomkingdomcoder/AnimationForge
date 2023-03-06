#include "Animation4ge.h"
#include "FileDialog.h"

using UI = UserInterface;
using UIGFCA = UI::Graphic<Animation>;
using UIGFCI = UI::Graphic<Image>;

void Animation4ge::Go()
{
	switch (state)
	{
		case State::MainMenu:
		{
			MainMenu();
			break;
		}
		case State::NewAnimation:
		{
			MessageBox(nullptr, "Sorry, but this feature has not yet been implemented.", "Error", MB_ICONEXCLAMATION | MB_OK);
			state = State::MainMenu;
			stateChange = false;
			break;
		}
		case State::ImportAnimation:
		{
			ImportAnimation();
			break;
		}
		case State::ExportAnimation:
		{
			MessageBox(nullptr, "Sorry, but this feature has not yet been implemented.", "Error", MB_ICONEXCLAMATION | MB_OK);
			state = State::MainMenu;
			stateChange = false;
			break;
		}
		case State::EditAnimation:
		{
			MessageBox(nullptr, "Sorry, but this feature has not yet been implemented.", "Error", MB_ICONEXCLAMATION | MB_OK);
			state = State::MainMenu;
			stateChange = false;
			break;
		}
	}
}

void Animation4ge::MainMenu()
{
	if (stateChange)
	{
		gfx.Erase(TextLayer);
		gfx.Erase(UILayer);
		gfx.Erase(BackgroundLayer);
		mainMenuInterface.EnableAll();
		Image("graphics\\banner-800x100.bmp").Draw(gfx, 0, 0, UILayer);
		stateChange = false;
	}
	float time = clock.Mark();
	mainMenuInterface.Update(time);
	mainMenuInterface.Draw();
}

void Animation4ge::ImportAnimation()
{
	if (stateChange)
	{
		gfcText.ResetDefaults();
		gfx.Erase(UILayer);
		gfx.Erase(BackgroundLayer);
		std::vector<std::wstring> wfiles = FileDialog::OpenFiles({ { L"Animations (.bmp)",L"*.bmp" } }, true, L"Choose an Animation Forge Animation File (.bmp)", L"Animate", L"Animation:");
		if (!wfiles.empty())
		{
			files.clear();
			files.resize(wfiles.size());
			for (int i = 0; i < wfiles.size(); ++i)
			{
				files[i] = FileDialog::ConvertWString(wfiles[i]);
			}

			std::ostringstream oss;
			oss << "Please indicate the width & height of each frame." << std::endl << std::endl << MaxAnimationFrameDim << " x " << MaxAnimationFrameDim << " maximum.";
			MessageBox(nullptr, oss.str().c_str(), "Import Successful", MB_ICONINFORMATION | MB_OK);
			gfcText.Write("DoubleCl!ck -> Submit Dimensions\r");
			gfcText.Write("Imported Animation X of X:");
			animImportIndex = 0;
			newAnim = true;

			stateChange = false;
		}
		else
		{
			MessageBox(nullptr, "No .bmp image files chosen!", "Import Error", MB_ICONEXCLAMATION | MB_OK);
		}
	}
	if (animImportIndex < files.size())
	{
		if (newAnim)
		{
			gfx.Erase(BackgroundLayer);
			curImportImg = Image(files[animImportIndex].c_str());
			if (curImportImg.GetWidth() > MaxAnimationFrameDim)
			{
				curImportImg.Crop(MaxAnimationFrameDim, curImportImg.GetHeight(), 0, 0);
			}
			if (curImportImg.GetHeight() > MaxAnimationFrameDim)
			{
				curImportImg.Crop(curImportImg.GetWidth(), MaxAnimationFrameDim, 0, 0);
			}
			curImportImg.AdjustSize((float)FrameZoom, (float)FrameZoom);
			imgPos = { (WindowWidth - curImportImg.GetWidth()) / 2,(WindowHeight - curImportImg.GetHeight()) / 2 + 16 };
			curImportImg.Draw(gfx, imgPos.x, imgPos.y, BackgroundLayer);

			sizeSelector = iRect(imgPos, curImportImg.GetWidth(), curImportImg.GetHeight());
			mousePos = { curImportImg.GetWidth(),curImportImg.GetHeight() };
			imgRect = sizeSelector;

			gfcText.SetCursorPosition({ 19,1 });
			std::ostringstream oss;
			oss << (animImportIndex + 1) << " of " << files.size() << ":   ";
			gfcText.Write(oss.str());

			newAnim = false;
		}
		gfcText.SetCursorPosition({ 0,2 });
		gfcText.SetTextColor(Colors::BrightRed);
		std::ostringstream oss;
		oss << "( " << (mousePos.x / FrameZoom) << " , " << (mousePos.y / FrameZoom) << " )   ";
		gfcText.Write(oss.str());
		gfcText.SetTextColor(Colors::White);

		if (imgRect.ContainsPoint({ mouse.GetX(),mouse.GetY() }))
		{
			mousePos = { mouse.GetX() - imgPos.x + FrameZoom,mouse.GetY() - imgPos.y + FrameZoom };
		}
		else
		{
			mousePos = { curImportImg.GetWidth(),curImportImg.GetHeight() };
		}
		while (mouse.isActive())
		{
			const Mouse::Event& me = mouse.Read();
			if (me.isLeftDoubleClick())
			{
				frameDim = mousePos;
				Image spriteSheet = Image(files[animImportIndex].c_str());
				animations.emplace_back(Animation(spriteSheet, frameDim, { spriteSheet.GetWidth() / frameDim.x,spriteSheet.GetHeight() / frameDim.y }, DefaultFPS));
				animImportIndex++;
				newAnim = true;
			}
		}
		sizeSelector.SetWidth(mousePos.x);
		sizeSelector.SetHeight(mousePos.y);
		sizeSelector.DrawOutline(gfx, FrameSelectorColor, MainLayer);
	}
	else
	{
		state = State::MainMenu;
		stateChange = true;
	}
}

Animation4ge::Animation4ge(Window& wnd)
	:
	clock(),
	wnd(wnd),
	mouse(wnd.mouse),
	kbd(wnd.kbd),
	gfx(wnd.gfx()),
	gfcText(gfx.GetPixelMap(TextLayer).data(), (vec2i(TextLayerRes)).GetVStruct()),
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
	gfx.ManuallyManage(BackgroundLayer);
	clock.Mark();
}

