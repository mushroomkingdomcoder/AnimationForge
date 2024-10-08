#include "Animation4ge.h"
#include "FileDialog.h"

using UI = UserInterface;
using UIGFCA = UI::Graphic<Animation>;
using UIGFCI = UI::Graphic<Image>;

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
	brefs( { mouse,state,stateChange,this->wnd } ),
	mbd({ { brefs,false },{ brefs,false },{ brefs,false } }),
	pViewAnimationsBtn
	(	std::make_unique<UIGFCA>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - 275, Animation("graphics\\view-anims-btn-1200x100-4x1.bmp", { 300,100 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			UIButtonData& mbd = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (fRect(
					vec2((float)thisGfc.x, (float)thisGfc.y) * mbd.refs.wnd.GetStretch(), 
					(vec2)thisGfc.GetGraphic().GetFrameSize() * mbd.refs.wnd.GetStretch()).ContainsPoint(vec2(mbd.refs.mouse.GetX(), mbd.refs.mouse.GetY())))
			{
				if (!mbd.hovering)
				{
					mbd.hovering = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
					}
					update = true;
				}
				if (mbd.refs.mouse.LeftIsClicked())
				{
					mbd.refs.appState = State::ViewAnimations;
					mbd.refs.appStateChange = true;
				}
			}
			else
			{
				if (mbd.hovering)
				{
					mbd.hovering = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
					}
					update = true;
				}
			}
			return update;
		}, 
		reinterpret_cast<char*>(&mbd[0]))
	),
	pImportAnimationBtn
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - 150, Image("graphics\\import-anim-btn-300x100.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& mbd = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			if ((fRect(thisGfc.GetGraphic().GetRect(
					thisGfc.x * mbd.refs.wnd.GetStretch().x, thisGfc.y * mbd.refs.wnd.GetStretch().y)) * mbd.refs.wnd.GetStretch()).ContainsPoint(vec2(mbd.refs.mouse.GetX(), mbd.refs.mouse.GetY())))
			{
				if (!mbd.hovering)
				{
					mbd.hovering = true;
					thisGfc.graphic.Rotate180();
					return true;
				}
				if (mbd.refs.mouse.LeftIsClicked())
				{
					mbd.refs.appState = State::ImportAnimation;
					mbd.refs.appStateChange = true;
				}
			}
			else
			{
				if (mbd.hovering)
				{
					mbd.hovering = false;
					thisGfc.graphic.Rotate180();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&mbd[1]))
	),
	pBackButton
	(	std::make_unique<UIGFCA>(
		0, 0, Animation("graphics\\back-btn-600x50-4x1.bmp", { 150,50 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			UIButtonData& mbd = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (mbd.refs.mouse.GetX() < thisGfc.GetGraphic().GetFrameWidth() * mbd.refs.wnd.GetStretch().x
				&& mbd.refs.mouse.GetY() < thisGfc.GetGraphic().GetFrameHeight() * mbd.refs.wnd.GetStretch().y)
			{
				if (!mbd.hovering)
				{
					mbd.hovering = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.InvertColors();
					}
					update = true;
				}
				if (mbd.refs.mouse.LeftIsClicked() && mbd.refs.mouse.GetX() >= 0 && mbd.refs.mouse.GetY() >= 0)
				{
					mbd.refs.appState = State::MainMenu;
					mbd.refs.appStateChange = true;
				}
			}
			else
			{
				if (mbd.hovering)
				{
					mbd.hovering = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.InvertColors();
					}
					update = true;
				}
			}
			return update;
		},
		reinterpret_cast<char*>(&mbd[2]))
	),
	mainMenuInterface(gfx, "charsets\\default.bmp", { 16,6 }, ' ', UILayer)
{
	mainMenuInterface.AddInterface(pViewAnimationsBtn);
	mainMenuInterface.AddInterface(pImportAnimationBtn);
	mainMenuInterface.AddInterface(pBackButton);
	gfx.SetBackgroundColor(BackgroundColor);
	gfx.ManuallyManage(TextLayer);
	gfx.ManuallyManage(UILayer);
	gfx.ManuallyManage(BackgroundLayer);
	kbd.DisableAutorepeat();
	clock.Mark();
}

void Animation4ge::Go()
{
	if (kbd.isActive())
	{
		auto event = kbd.Read();
		if (event.isKeyUp() && event.GetKeycode() == 'F')
		{
			if (wnd.isPseudoFullscreen())
			{
				wnd.ResetWindow();
			}
			else
			{
				wnd.SetPseudoFullscreen();
			}
		}
	}

	switch (state)
	{
		case State::MainMenu:
		{
			MainMenu();
			break;
		}
		case State::ViewAnimations:
		{
			ViewAnimations();
			break;
		}
		case State::ImportAnimation:
		{
			ImportAnimation();
			break;
		}
	}
}

void Animation4ge::ResetDisplay()
{
	gfx.Erase(UILayer);
	gfx.Erase(TextLayer);
	gfx.Erase(BackgroundLayer);
	gfcText.ResetDefaults();
}

void Animation4ge::MainMenu()
{
	if (stateChange)
	{
		ResetDisplay();
		mainMenuInterface.DisableAll();
		mainMenuInterface.EnableInterface((int)UIs::ViewAnimBtn);
		mainMenuInterface.EnableInterface((int)UIs::ImportAnimBtn);
		Image("graphics\\banner-800x100.bmp").Draw(gfx, UILayerRes.x / 2 - 400, 0, UILayer);
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
		ResetDisplay();
		std::vector<std::wstring> wfiles = FileDialog::OpenFiles({ { L"Animations (.bmp)",L"*.bmp" } }, true, L"Choose an Animation Forge Animation File (.bmp)", L"Animate", L"Animation:");
		if (!wfiles.empty())
		{
			if (wfiles.size() + animations.size() > MaxAnimations)
			{
				std::ostringstream oss;
				oss << "This application cannot edit more than " << MaxAnimations << " at a time.\nRight now, " << animations.size() << " animations are loaded and you attempted to load "
					<< wfiles.size() << " more, totaling " << animations.size() + wfiles.size() << " animations.\nPlease remove (and export if applicable) some animations from the app or select fewer to import.";
				MessageBox(nullptr, oss.str().c_str(), "Error", MB_ICONEXCLAMATION | MB_OK);
			}
			else
			{
				files.clear();
				files.resize(wfiles.size());
				for (int i = 0; i < wfiles.size(); ++i)
				{
					files[i] = FileDialog::ConvertWString(wfiles[i]);
				}

				std::ostringstream oss;
				oss << "Please indicate the width & height of each frame.\n\n"
					<< MaxAnimationFrameDim.x << " x " << MaxAnimationFrameDim.y << " frame dimension maximum.\n"
					<< MaxFramesPerAnimation << " frame count maximum.";
				MessageBox(nullptr, oss.str().c_str(), "Success", MB_ICONINFORMATION | MB_OK);
				gfcText.Write("                 DoubleCl!ck -> Submit Dimensions, SPACE -> skip\r");
				gfcText.Write("                          Imported Animation X of X:");
				animImportIndex = 0;
				newAnim = true;
				skipAnim = false;

				stateChange = false;
			}
		}
		else
		{
			MessageBox(nullptr, "No .bmp image files chosen!", "Error", MB_ICONEXCLAMATION | MB_OK);
		}
	}
	if (animImportIndex < files.size())
	{
		if (newAnim)
		{
			gfx.Erase(BackgroundLayer);
			curImportImg = Image(files[animImportIndex].c_str());
			bool animTooLarge = false;
			if (curImportImg.GetWidth() < MaxAnimationFrameDim.x && curImportImg.GetHeight() / MaxAnimationFrameDim.y > MaxFramesPerAnimation)
			{
				animTooLarge = true;
			}
			else if (curImportImg.GetHeight() < MaxAnimationFrameDim.y && curImportImg.GetWidth() / MaxAnimationFrameDim.x > MaxFramesPerAnimation)
			{
				animTooLarge = true;
			}
			else if ((curImportImg.GetWidth() / MaxAnimationFrameDim.x) * (curImportImg.GetHeight() / MaxAnimationFrameDim.y) > MaxFramesPerAnimation)
			{
				animTooLarge = true;
			}
			if (animTooLarge)
			{
				std::ostringstream oss;
				oss << "Animation \"" << files[animImportIndex] << "\" is too large to import and will be ignored. Please specify the frame dimensions of the next animation file.";
				MessageBox(nullptr, oss.str().c_str(), "Error", MB_ICONEXCLAMATION | MB_OK);
				skipAnim = true;
			}
			else
			{
				if (curImportImg.GetWidth() > MaxAnimationFrameDim.x)
				{
					curImportImg.Crop(MaxAnimationFrameDim.x, curImportImg.GetHeight(), 0, 0);
				}
				if (curImportImg.GetHeight() > MaxAnimationFrameDim.y)
				{
					curImportImg.Crop(curImportImg.GetWidth(), MaxAnimationFrameDim.y, 0, 0);
				}
				curImportImg.AdjustSize((float)FrameZoom.x, (float)FrameZoom.y);
				imgPos = { (WindowWidth - curImportImg.GetWidth()) / 2,(WindowHeight - curImportImg.GetHeight()) / 2 + 16 };
				curImportImg.Draw(gfx, imgPos.x, imgPos.y, BackgroundLayer);

				sizeSelector = iRect(imgPos, curImportImg.GetWidth(), curImportImg.GetHeight());
				mousePos = { curImportImg.GetWidth(),curImportImg.GetHeight() };
				imgRect = sizeSelector;

				gfcText.SetCursorPosition({ 45,1 });
				std::ostringstream oss;
				oss << (animImportIndex + 1) << " of " << files.size() << ":   ";
				gfcText.Write(oss.str());

				newAnim = false;
			}
		}
		if (!skipAnim)
		{
			gfcText.SetCursorPosition({ 0,2 });
			gfcText.SetTextColor(Colors::LightCyan);
			std::ostringstream oss;
			oss << "( " << (mousePos.x / FrameZoom.x) << " , " << (mousePos.y / FrameZoom.y) << " )   ";
			gfcText.Write(oss.str());
			gfcText.SetTextColor(Colors::White);

			if (!imgRect.ContainsPoint({ int(float(mouse.GetX()) / wnd.GetStretch().x),imgRect.GetPosition().y}))
			{
				mousePos.x = curImportImg.GetWidth();
			}
			else
			{
				mousePos.x = int(float(mouse.GetX()) / wnd.GetStretch().x) - imgPos.x + FrameZoom.x;
			}
			if (!imgRect.ContainsPoint({ imgRect.GetPosition().x,int(float(mouse.GetY()) / wnd.GetStretch().y) }))
			{
				mousePos.y = curImportImg.GetHeight();
			}
			else
			{
				mousePos.y = int(float(mouse.GetY()) / wnd.GetStretch().y) - imgPos.y + FrameZoom.y;
			}

			while (mouse.isActive())
			{
				const Mouse::Event me = mouse.Read();
				if (me.isLeftDoubleClick())
				{
					frameDim = { mousePos.x / FrameZoom.x,mousePos.y / FrameZoom.y };
					Image spriteSheet = Image(files[animImportIndex].c_str());
					int2 sheetDim = { spriteSheet.GetWidth() / frameDim.x,spriteSheet.GetHeight() / frameDim.y };
					if (sheetDim.x * sheetDim.y <= MaxFramesPerAnimation)
					{
						animations.emplace_back(Animation(spriteSheet, frameDim, sheetDim, DefaultFPS));
						animImportIndex++;
						newAnim = true;
					}
				}
			}
			sizeSelector.SetWidth(mousePos.x);
			sizeSelector.SetHeight(mousePos.y);
			sizeSelector.DrawOutline(gfx, FrameSelectorColor, MainLayer);
			while (kbd.isActive())
			{
				const Keyboard::Event ke = kbd.Read();
				if (ke.isKeyDown() && ke.GetKeycode() == VK_SPACE)
				{
					skipAnim = true;
				}
			}
		}
		else
		{
			animImportIndex++;
			newAnim = true;
			skipAnim = false;
		}
	}
	else
	{
		files.clear();
		state = State::MainMenu;
		stateChange = true;
	}
}

void Animation4ge::ViewAnimations()
{
	if (animations.empty())
	{
		MessageBox(nullptr, "There are currently no animations to view :(\nImport a couple first to get started!", "Error", MB_ICONEXCLAMATION | MB_OK);
		state = State::MainMenu;
		stateChange = false;
		return;
	}
	if (stateChange)
	{
		ResetDisplay();
		mainMenuInterface.DisableAll();
		mainMenuInterface.EnableInterface((int)UIs::BackBtn);

		

		stateChange = false;
	}
	float time = clock.Mark();
	mainMenuInterface.Update(time);
	mainMenuInterface.Draw();
	animations[nAnimation].Play(time);
	int drawX = (MainLayerRes.x / 2) - (animations[nAnimation].GetFrameWidth() / 2);
	int drawY = (MainLayerRes.y / 2) - (animations[nAnimation].GetFrameHeight() / 2);
	animations[nAnimation].Draw(gfx, drawX, drawY, MainLayer);
}
