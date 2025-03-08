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
	bdata({ { mouse,state,stateChange,this->wnd,paused,zoomed,nAnimation,&pCurAnimation,animations },{} }),
	pViewAnimationsBtn
	(	std::make_unique<UIGFCA>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - 275, Animation(L"graphics\\view-anims-btn-1200x100-4x1.bmp", { 300,100 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::ViewAnimBtn;
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (fRect(
					vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(), 
					(vec2)thisGfc.GetGraphic().GetFrameSize() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					bdata.hovering[id] = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
					}
					update = true;
				}
				if (bdata.refs.mouse.LeftIsClicked())
				{
					bdata.refs.appState = State::ViewAnimations;
					bdata.refs.appStateChange = true;
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.Rotate180();
					}
					update = true;
				}
			}
			return update;
		}, 
		reinterpret_cast<char*>(&bdata))
	),
	pImportAnimationBtn
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 150, (UILayerRes.y + 100) / 2 - 150, Image(L"graphics\\import-anim-btn-300x100.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::ImportAnimBtn;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					bdata.hovering[id] = true;
					thisGfc.graphic.Rotate180();
					return true;
				}
				if (bdata.refs.mouse.LeftIsClicked())
				{
					bdata.refs.appState = State::ImportAnimation;
					bdata.refs.appStateChange = true;
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.Rotate180();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pBackButton
	(	std::make_unique<UIGFCA>(
		0, 0, Animation(L"graphics\\back-btn-600x50-4x1.bmp", { 150,50 }, { 4,1 }, 4),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCA& thisGfc = reinterpret_cast<UIGFCA&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::BackBtn;
			bool update = thisGfc.graphic.PlayAndCheck(time);
			if (bdata.refs.mouse.GetX() < thisGfc.GetGraphic().GetFrameWidth() * bdata.refs.wnd.GetStretch().x
				&& bdata.refs.mouse.GetY() < thisGfc.GetGraphic().GetFrameHeight() * bdata.refs.wnd.GetStretch().y)
			{
				if (!bdata.hovering[id])
				{
					bdata.hovering[id] = true;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.InvertColors();
					}
					update = true;
				}
				if (bdata.refs.mouse.LeftIsClicked() && bdata.refs.mouse.GetX() >= 0 && bdata.refs.mouse.GetY() >= 0)
				{
					bdata.refs.appState = State::MainMenu;
					bdata.refs.appStateChange = true;
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					for (Image& frame : thisGfc.graphic.GetFrames())
					{
						frame.InvertColors();
					}
					update = true;
				}
			}
			return update;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pPlayPause
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 25, 0, Image(L"graphics\\pause-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::PlayPause;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (bdata.refs.paused)
						{
							bdata.refs.paused = false;
							thisGfc.graphic.Load(L"graphics\\pause-50x50.bmp").InvertColors();
						}
						else
						{
							bdata.refs.paused = true;
							thisGfc.graphic.Load(L"graphics\\play-50x50.bmp").InvertColors();
						}
						return true;
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pSlow
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 - 100, 0, Image(L"graphics\\slow-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::Slow;
			float curFPS = (*bdata.refs.ppCurAnimation)->GetFPS();
			bool minFPS = curFPS < MinFPS;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					if (minFPS) return false;
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (minFPS) return false;
						(*bdata.refs.ppCurAnimation)->SetFPS(curFPS / StepFPS);
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pFast
	(	std::make_unique<UIGFCI>(
		UILayerRes.x / 2 + 50, 0, Image(L"graphics\\fast-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::Fast;
			float curFPS = (*bdata.refs.ppCurAnimation)->GetFPS();
			bool maxFPS = curFPS > MaxFPS;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					if (maxFPS) return false;
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (maxFPS) return false;
						(*bdata.refs.ppCurAnimation)->SetFPS(curFPS * StepFPS);
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pPrev
	(	std::make_unique<UIGFCI>(
		0, UILayerRes.y / 2 - 25, Image(L"graphics\\prev-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::Prev;
			bool firstAnim = bdata.refs.nAnimation == 0;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					if (firstAnim) return false;
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (firstAnim) return false;
						--bdata.refs.nAnimation;
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pNext
	(	std::make_unique<UIGFCI>(
		UILayerRes.x - 50, UILayerRes.y / 2 - 25, Image(L"graphics\\next-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::Next;
			bool lastAnim = bdata.refs.nAnimation == bdata.refs.animations.size() - 1;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					if (lastAnim) return false;
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (lastAnim) return false;
						++bdata.refs.nAnimation;
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	pZoom
	(	std::make_unique<UIGFCI>(
		UILayerRes.x - 50, 0, Image(L"graphics\\zoomout-50x50.bmp"),
		[](std::unique_ptr<UI::Object>& thisObj, float time) -> bool
		{
			UIGFCI& thisGfc = reinterpret_cast<UIGFCI&>(*thisObj);
			UIButtonData& bdata = reinterpret_cast<UIButtonData&>(*thisGfc.pData);
			const int id = (int)UIs::Zoom;
			if (fRect(
				vec2((float)thisGfc.x, (float)thisGfc.y) * bdata.refs.wnd.GetStretch(),
				(vec2)thisGfc.GetGraphic().GetDimensions() * bdata.refs.wnd.GetStretch()).ContainsPoint(vec2((float)bdata.refs.mouse.GetX(), (float)bdata.refs.mouse.GetY())))
			{
				if (!bdata.hovering[id])
				{
					bdata.hovering[id] = true;
					thisGfc.graphic.InvertColors();
					return true;
				}
				while (bdata.refs.mouse.isActive())
				{
					if (bdata.refs.mouse.Read().isLeftClick())
					{
						if (bdata.refs.zoomed)
						{
							bdata.refs.zoomed = false;
							thisGfc.graphic.Load(L"graphics\\zoomin-50x50.bmp").InvertColors();
							
						}
						else
						{
							bdata.refs.zoomed = true;
							thisGfc.graphic.Load(L"graphics\\zoomout-50x50.bmp").InvertColors();
						}
						return true;
					}
				}
			}
			else
			{
				if (bdata.hovering[id])
				{
					bdata.hovering[id] = false;
					thisGfc.graphic.InvertColors();
					return true;
				}
			}
			return false;
		},
		reinterpret_cast<char*>(&bdata))
	),
	mainMenuInterface(gfx, L"charsets\\default.bmp", { 16,6 }, ' ', UILayer)
{
	mainMenuInterface.AddInterfaces( { &pViewAnimationsBtn,&pImportAnimationBtn,&pBackButton,&pPlayPause,&pSlow,&pFast,&pPrev,&pNext,&pZoom });
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
		mainMenuInterface.EnableInterfaces({ (int)UIs::ViewAnimBtn,(int)UIs::ImportAnimBtn });
		Image(L"graphics\\banner-800x100.bmp").Draw(gfx, UILayerRes.x / 2 - 400, 0, UILayer);
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
		wfiles = FileDialog::OpenFiles({ { L"Animations (.bmp)",L"*.bmp" } }, true, L"Choose an Animation Forge Animation File (.bmp)", L"Animate", L"Animation:");
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
				std::ostringstream oss;
				oss << "Please indicate the width & height of each frame.\n\n"
					<< MaxAnimationFrameDim.x << " x " << MaxAnimationFrameDim.y << " frame dimension maximum.\n"
					<< MaxFramesPerAnimation << " frame count maximum.";
				MessageBox(nullptr, oss.str().c_str(), "Success", MB_ICONINFORMATION | MB_OK);
				gfcText.Write("                 DoubleClick -> Submit Dimensions, SPACE -> skip\r");
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
	if (animImportIndex < wfiles.size())
	{
		if (newAnim)
		{
			gfx.Erase(BackgroundLayer);
			curImportImg = Image(wfiles[animImportIndex].c_str());
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
				std::wostringstream oss;
				oss << "Animation \"" << wfiles[animImportIndex] << "\" is too large to import and will be ignored. Please specify the frame dimensions of the next animation file.";
				MessageBoxW(nullptr, oss.str().c_str(), L"Error", MB_ICONEXCLAMATION | MB_OK);
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
				oss << (animImportIndex + 1) << " of " << wfiles.size() << ":   ";
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
					Image spriteSheet = Image(wfiles[animImportIndex].c_str());
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
		wfiles.clear();
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
		mainMenuInterface.EnableInterfaces({ (int)UIs::BackBtn,(int)UIs::PlayPause,(int)UIs::Slow,(int)UIs::Fast,(int)UIs::Prev,(int)UIs::Next,(int)UIs::Zoom });
		stateChange = false;

		gfcText.SetCursorPosition({ 32,TextLayerRes.y / 16 - 2 });
		gfcText.SetTextColor(Colors::BrightCyan);
		std::ostringstream oss;
		if (animations.size() < 10)
		{
			oss << "Animation XX of 0" << animations.size();
		}
		else
		{
			oss << "Animation XX of " << animations.size();
		}
		gfcText.Write(oss.str());
		gfcText.SetCursorPosition((vec2i(gfcText.GetCursorPosition()) - vec2i{ 8, 0 }).GetVStruct());

		zoomed = true;
		gfx.SetViewport(-(gfx.GetViewWidth(MainLayer) / 2), -(gfx.GetViewHeight(MainLayer) / 2), gfx.GetViewWidth(MainLayer) * 2, gfx.GetViewHeight(MainLayer) * 2, MainLayer);
	}
	pCurAnimation = &animations[nAnimation];

	bool prevZS = zoomed;

	float time = clock.Mark();
	mainMenuInterface.Update(time);
	mainMenuInterface.Draw();

	if (!(zoomed == prevZS))
	{
		if (zoomed)
		{
			gfx.SetViewport(-(gfx.GetViewWidth(MainLayer) / 2), -(gfx.GetViewHeight(MainLayer) / 2), gfx.GetViewWidth(MainLayer) * 2, gfx.GetViewHeight(MainLayer) * 2, MainLayer);
		}
		else
		{
			gfx.SetViewport(0, 0, gfx.GetViewWidth(MainLayer) / 2, gfx.GetViewHeight(MainLayer) / 2, MainLayer);
		}
	}

	if (!paused) animations[nAnimation].Play(time);

	std::ostringstream oss;
	if (nAnimation < 9)
	{
		oss << "0" << (nAnimation + 1);
	}
	else
	{
		oss << (nAnimation + 1);
	}
	gfcText.Write(oss.str());
	gfcText.CursorLeft(); gfcText.CursorLeft();

	int drawX = (MainLayerRes.x / 2) - (animations[nAnimation].GetFrameWidth() / 2);
	int drawY = (MainLayerRes.y / 2) - (animations[nAnimation].GetFrameHeight() / 2);
	animations[nAnimation].Draw(gfx, drawX, drawY, MainLayer);

	if (stateChange)
	{
		if (zoomed) gfx.SetViewport(0, 0, gfx.GetViewWidth(MainLayer) / 2, gfx.GetViewHeight(MainLayer) / 2, MainLayer); zoomed = false;
	}
}








