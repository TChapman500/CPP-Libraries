#pragma once
#include "UIElement.h"
#include <string>

namespace TChapman500 {
namespace GUI {

class PregeneratedText;
class UIText : public UIElement
{
public:
	UIText(UISystem *uiSystem);
	~UIText();

	virtual void Render(float deltaTime) override;

	virtual bool SetSize(float x, float y) override;
	virtual bool SetAnchors(float minX, float minY, float maxX, float maxY) override;
	virtual bool SetOffsets(float minX, float minY, float maxX, float maxY) override;

	virtual bool AddChild(const std::shared_ptr<UIElement> &child) override;
	virtual bool RemoveChild(size_t index) override;

	std::shared_ptr<PregeneratedText> GetPregeneratedText();
	bool SetPregeneratedText(std::shared_ptr<PregeneratedText> &pregenerated, bool ownsPregen = false);

	bool SetText(std::wstring &text);

	bool RegenerateOnResize;

private:
	std::shared_ptr<PregeneratedText> _PregenText;
	bool _OwnsPregen;
};

}}
