#include "UIText.h"
#include "UISystem.h"
#include "PregeneratedText.h"

namespace TChapman500 {
namespace GUI {

UIText::UIText(UISystem *uiSystem) : UIElement(uiSystem)
{
	_PregenText = std::make_shared<PregeneratedText>();
	_PregenText->Width = Size.X;
	_PregenText->Height = Size.Y;
	_OwnsPregen = true;
	HasFocus = false;
	HasMouseOver = false;
	RegenerateOnResize = true;
}

UIText::~UIText()
{
}

void UIText::Render(float deltaTime)
{
	matrix_buffer matrixData = { LocalToWorld, Window->WorldToScreen };
	Window->MatrixBuffer->WriteBuffer(&matrixData);
	element_buffer elementData = { Pivot, Size };
	Window->UIElementBuffer->WriteBuffer(&elementData);

	bool texture = true;

	material_buffer materialData = { _PregenText->Color, 1 };
	Window->MaterialBuffer->WriteBuffer(&materialData);


	UISystem::AdvancedRectVertexShader->Set();
	_PregenText->Render();
	UIElement::Render(deltaTime);
}

bool UIText::SetSize(float x, float y)
{
	if (!UIElement::SetSize(x, y)) return false;
	if (_OwnsPregen)
	{
		_PregenText->Width = x;
		_PregenText->Height = y;
		if (RegenerateOnResize) _PregenText->Generate(_PregenText->Text);
	}
	return true;
}

bool UIText::SetAnchors(float minX, float minY, float maxX, float maxY)
{
	if (!UIElement::SetAnchors(minX, minY, maxX, maxY)) return false;
	if (_OwnsPregen)
	{
		_PregenText->Width = Size.X;
		_PregenText->Height = Size.Y;
		if (RegenerateOnResize) _PregenText->Generate(_PregenText->Text);
	}
	return true;
}

bool UIText::SetOffsets(float minX, float minY, float maxX, float maxY)
{
	if (!UIElement::SetOffsets(minX, minY, maxX, maxY)) return false;
	if (_OwnsPregen)
	{
		_PregenText->Width = Size.X;
		_PregenText->Height = Size.Y;
		if (RegenerateOnResize) _PregenText->Generate(_PregenText->Text);
	}
	return true;
}

bool UIText::AddChild(const std::shared_ptr<UIElement> &child) { return false; }
bool UIText::RemoveChild(size_t index) { return false; }
std::shared_ptr<PregeneratedText> UIText::GetPregeneratedText() { return _PregenText; }

bool UIText::SetPregeneratedText(std::shared_ptr<PregeneratedText> &pregenerated, bool ownsPregen)
{
	if (!pregenerated) return false;
	if (pregenerated->Width != Size.X || pregenerated->Height != Size.Y) return false;
	_PregenText = pregenerated;
	_OwnsPregen = ownsPregen;
	return true;
}

bool UIText::SetText(std::wstring &text) { return _PregenText->Generate(text); }

}}
