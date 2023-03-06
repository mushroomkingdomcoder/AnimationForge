#include "UserInterface.h"

UserInterface::UserInterface(Graphics& gfx, std::string text_font, char2 char_table_dim, int start_char, int layer)
	:
	gfx(gfx),
	layer(layer),
	gfcText(gfx.GetPixelMap(layer).data(), gfx.GetDimensions(layer).GetVStruct(), Image(text_font.c_str()), char_table_dim, start_char),
	pObjects(),
	drawFlag(true)
{}

void UserInterface::AddInterface(std::unique_ptr<Object>& pInterface)
{
	assert(pInterface != nullptr);
	pObjects.emplace_back(std::move(pInterface));
}

void UserInterface::DisableInterface(int index)
{
	assert(index >= 0 && index < pObjects.size());
	pObjects[index]->isActive = false;
}

void UserInterface::EnableInterface(int index)
{
	assert(index >= 0 && index < pObjects.size());
	pObjects[index]->isActive = true;
}

void UserInterface::DisableAll()
{
	for (auto& pObject : pObjects)
	{
		pObject->isActive = false;
	}
}

void UserInterface::EnableAll()
{
	for (auto& pObject : pObjects)
	{
		pObject->isActive = true;
	}
}

bool UserInterface::InterfaceIsEnabled(int index) const
{
	assert(index >= 0 && index < pObjects.size());
	return pObjects[index]->isActive;
}

void UserInterface::Update(float time_ellapsed)
{
	for (auto& pObject : pObjects)
	{
		if (pObject->update && pObject->isActive)
		{
			if (pObject->update(pObject, time_ellapsed))
			{
				drawFlag = true;
			}
		}
	}
}

void UserInterface::Draw()
{
	if (drawFlag)
	{
		for (auto& pObject : pObjects)
		{
			if (pObject->isActive)
			{
				pObject->Draw(gfx, layer, gfcText);
			}
		}
		drawFlag = false;
	}
}
