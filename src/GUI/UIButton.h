#pragma once
#include "UIElement.h"
#include "UIImage.h"
#include "UIText.h"
#include <vectors.h>
#include "../Graphics/Textures.h"
#include <event.h>

namespace TChapman500 {
namespace GUI {

class UIButton : public UIElement
{
public:
	UIButton(UISystem *uiSystem);
	virtual ~UIButton();

	void SetInteractable(bool interactable);

	virtual void Render(float deltaTime) override;

	virtual bool GetPassive() override;

	virtual void OnKeyPressed(Input::key_code key) override;

	virtual void LButtonDown() override;
	virtual void LButtonUp() override;

	virtual void OnMouseOver() override;
	virtual void OnMouseOut() override;

	enum class response_type
	{
		ColorSwap,
		SpriteSwap,
		Animation
	};
	response_type ResponseType;
	enum class state
	{
		Normal,
		Highlighted,
		Pressed,
		Selected,
		Disabled
	};
	state State;
	state PrevState;
	state NextState;

	float FadeDuration;
	// Button Colors
	Math::vector4f NormalColor;
	Math::vector4f HighlightedColor;
	Math::vector4f PressedColor;
	Math::vector4f SelectedColor;
	Math::vector4f DisabledColor;
	// Button Sprites
	std::shared_ptr<Graphics::ShaderResourceView> NormalSprite;
	std::shared_ptr<Graphics::ShaderResourceView> HighlightedSprite;
	std::shared_ptr<Graphics::ShaderResourceView> PressedSprite;
	std::shared_ptr<Graphics::ShaderResourceView> SelectedSprite;
	std::shared_ptr<Graphics::ShaderResourceView> DisabledSprite;

	std::shared_ptr<UIImage> Image;
	std::shared_ptr<UIText> Text;
	bool Interactable;

	event Clicked;

private:
	float _FadeTime;
};

}}
