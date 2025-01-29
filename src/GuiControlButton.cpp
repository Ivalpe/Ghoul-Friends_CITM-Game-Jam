#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	posHitbox.setY(bounds.y);
	drawBasic = false;
	cursorAudio = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/UI/JDSherbert - Pixel UI SFX Pack - Cursor 1 (Square).ogg");
	selectAudio = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/UI/JDSherbert - Pixel UI SFX Pack - Select 1 (Sine).ogg");
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (show)
	{
		posHitbox.setX(bounds.x);
		posTexture.setX(-(Engine::GetInstance().render.get()->camera.x / 4) + bounds.x / 4);
		posTexture.setY(-(Engine::GetInstance().render.get()->camera.y / 4) + bounds.y / 4);
		if (state != GuiControlState::DISABLED) {

			Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

			//If the position of the mouse if inside the bounds of the button 
			if (mousePos.getX() > posHitbox.getX() / 4 && mousePos.getX() < (posHitbox.getX() + bounds.w) / 4 && mousePos.getY() > posHitbox.getY() / 4 && mousePos.getY() < (posHitbox.getY() + bounds.h) / 4) {

				if (state != GuiControlState::FOCUSED) {
					state = GuiControlState::FOCUSED;
					Engine::GetInstance().audio.get()->PlayFx(cursorAudio);
				}

				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
					state = GuiControlState::PRESSED;
					Engine::GetInstance().audio.get()->PlayFx(selectAudio);
				}


				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
					NotifyObserver();

			}
			else {
				state = GuiControlState::NORMAL;
			}
		}

		switch (state)
		{
		case GuiControlState::DISABLED:
			Engine::GetInstance().render->DrawTexture(buttonDisabled, posTexture.getX(), posTexture.getY(), SDL_FLIP_NONE);
			break;
		case GuiControlState::NORMAL:
			Engine::GetInstance().render->DrawTexture(buttonNormal, posTexture.getX(), posTexture.getY(), SDL_FLIP_NONE);
			break;
		case GuiControlState::FOCUSED:
			Engine::GetInstance().render->DrawTexture(buttonFocused, posTexture.getX(), posTexture.getY(), SDL_FLIP_NONE);
			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().render->DrawTexture(buttonPressed, posTexture.getX(), posTexture.getY(), SDL_FLIP_NONE);
			break;
		}
		std::string name = "  ";
		name = name + text.c_str() + name;
		Engine::GetInstance().render->DrawText(name.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);
	}

	return false;
}

