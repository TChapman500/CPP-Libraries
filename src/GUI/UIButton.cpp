#include "UIButton.h"


namespace TChapman500 {
namespace GUI {

UIButton::UIButton(UISystem *uiSystem) : UIElement(uiSystem)
{
	Passive = false;
	Interactable = true;
	ResponseType = response_type::ColorSwap;
	State = state::Normal;
	PrevState = State;
	NextState = State;
	FadeDuration = 0.0f;
	_FadeTime = 0.0f;
}

UIButton::~UIButton()
{

}

void UIButton::SetInteractable(bool interactable)
{
	Interactable = interactable;
	if (interactable)
	{
		State = state::Normal;
		PrevState = state::Normal;
		NextState = state::Normal;
		switch (ResponseType)
		{
		case response_type::ColorSwap:
			if (Image) Image->Color = NormalColor;
			break;
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(NormalSprite);
			break;
		case response_type::Animation:
			break;
		}
	}
	else
	{
		State = state::Disabled;
		PrevState = state::Disabled;
		NextState = state::Disabled;
		switch (ResponseType)
		{
		case response_type::ColorSwap:
			if (Image) Image->Color = DisabledColor;
			break;
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(DisabledSprite);
			break;
		case response_type::Animation:
			break;
		}
	}
}

void UIButton::Render(float deltaTime)
{
	// TODO:  Add Animation rendering logic.

	if (PrevState != State)
	{
		if (FadeDuration > 0.0f)
		{
			float ratio = _FadeTime / FadeDuration;
			switch (ResponseType)
			{
			case response_type::ColorSwap:
				if (Image)
				{
					Math::vector4f color1, color2;
					// Calculate Previous State Weight
					switch (PrevState)
					{
					case state::Normal:
						color1 = NormalColor * ratio;
						break;
					case state::Highlighted:
						color1 = HighlightedColor * ratio;
						break;
					case state::Pressed:
						color1 = PressedColor * ratio;
						break;
					case state::Selected:
						color1 = SelectedColor * ratio;
						break;
					case state::Disabled:
						color1 = DisabledColor * ratio;
						break;
					}
					// Calculate Current State Weight
					switch (State)
					{
					case state::Normal:
						color2 = NormalColor * (1.0f - ratio);
						break;
					case state::Highlighted:
						color2 = HighlightedColor * (1.0f - ratio);
						break;
					case state::Pressed:
						color2 = PressedColor * (1.0f - ratio);
						break;
					case state::Selected:
						color2 = SelectedColor * (1.0f - ratio);
						break;
					case state::Disabled:
						color2 = DisabledColor * (1.0f - ratio);
						break;
					}
					// Calculate Final Color
					Image->Color = color1 + color2;
				}
				break;
			case response_type::SpriteSwap:
				break;
			case response_type::Animation:
				break;
			}

			_FadeTime += deltaTime;
			if (_FadeTime >= FadeDuration)
			{
				_FadeTime = 0.0f;
				PrevState = State;
				State = NextState;
			}
		}
		else
		{
			PrevState = State;
			State = NextState;

			switch (ResponseType)
			{
			case response_type::ColorSwap:
				if (Image)
				{
					switch (State)
					{
					case state::Normal:
						Image->Color = NormalColor;
						break;
					case state::Highlighted:
						Image->Color = HighlightedColor;
						break;
					case state::Pressed:
						Image->Color = PressedColor;
						break;
					case state::Selected:
						Image->Color = SelectedColor;
						break;
					case state::Disabled:
						Image->Color = DisabledColor;
						break;
					}
				}
				break;
			case response_type::SpriteSwap:
				break;
			case response_type::Animation:
				break;
			}
		}
	}
	else if (State != NextState) State = NextState;

	UIElement::Render(deltaTime);
}

bool UIButton::GetPassive() { return false; }

void UIButton::OnKeyPressed(Input::key_code key)
{
	if (HasFocus)
	{
		State = state::Pressed;
		NextState = state::Selected;
		switch (ResponseType)
		{
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(PressedSprite);
			break;
		case response_type::Animation:
			break;
		}
		Clicked.fire();
	}
}

void UIButton::LButtonDown()
{
	if (!Interactable) return;

	if (HasMouseOver)
	{
		HasFocus = true;

		State = state::Pressed;
		NextState = state::Pressed;
		switch (ResponseType)
		{
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(PressedSprite);
			break;
		case response_type::Animation:
			break;
		}
	}
	else
	{
		HasFocus = false;

		State = state::Normal;
		NextState = state::Normal;
		switch (ResponseType)
		{
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(NormalSprite);
			break;
		case response_type::Animation:
			break;
		}
	}

}

void UIButton::LButtonUp()
{
	if (!Interactable) return;

	// Left mouse button was pressed while mouse was over the control
	if (State == state::Pressed)
	{
		State = state::Selected;
		NextState = state::Selected;
		switch (ResponseType)
		{
		case response_type::SpriteSwap:
			if (Image) Image->SetTexture(SelectedSprite);
			break;
		case response_type::Animation:
			break;
		}

		// Mouse is still over the control
		if (HasMouseOver) Clicked.fire();
	}
}

void UIButton::OnMouseOver()
{
	if (!Interactable) return;
	if (State == state::Selected) return;

	State = state::Highlighted;
	NextState = state::Highlighted;
	switch (ResponseType)
	{
	case response_type::SpriteSwap:
		if (Image) Image->SetTexture(HighlightedSprite);
		break;
	case response_type::Animation:
		break;
	}
}

void UIButton::OnMouseOut()
{
	if (!Interactable) return;
	switch (State)
	{
	case state::Selected:
	case state::Pressed:
		return;
	}

	State = state::Normal;
	NextState = state::Normal;
	switch (ResponseType)
	{
	case response_type::SpriteSwap:
		if (Image) Image->SetTexture(NormalSprite);
		break;
	case response_type::Animation:
		break;
	}
}




}
}
