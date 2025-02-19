#pragma once
#include <vectors.h>
#include <vector>
#include <memory>
#include "../InputSystem/input.h"

namespace TChapman500 {
namespace GUI {

class UISystem;
class UIElement : public std::enable_shared_from_this<UIElement>
{
public:
	UIElement(UISystem *uiSystem);
	virtual ~UIElement();

	virtual void Render(float deltaTime);

	virtual bool SetPosition(float x, float y);
	virtual bool SetSize(float x, float y);
	virtual bool SetAnchors(float minX, float minY, float maxX, float maxY);
	virtual bool SetOffsets(float minX, float minY, float maxX, float maxY);
	virtual bool SetPivot(float x, float y);
	virtual bool SetScale(float x, float y);
	virtual bool SetRotation(float angle);

protected:
	virtual bool IsValidAsChildObject(const std::shared_ptr<UIElement> &child);
	virtual bool AddChild(const std::shared_ptr<UIElement> &child);

public:
	virtual bool RemoveChild(size_t index);
	std::shared_ptr<UIElement> GetChild(size_t index);
	size_t GetChildIndex(const std::shared_ptr<UIElement> &child);
	bool MoveChild(size_t from, size_t to);
	size_t GetChildCount();

	virtual void SetActive(bool active);
	virtual bool GetPassive();

	std::weak_ptr<UIElement> GetParent();
	bool SetParent(const std::shared_ptr<UIElement> &parent);

	virtual void OnKeyPressed(Input::key_code key);
	virtual void OnKeyReleased(Input::key_code key);

	virtual void LButtonDown();
	virtual void LButtonUp();
	virtual void MButtonDown();
	virtual void MButtonUp();
	virtual void RButtonDown();
	virtual void RButtonUp();

	virtual void X1ButtonDown();
	virtual void X1ButtonUp();
	virtual void X2ButtonDown();
	virtual void X2ButtonUp();

	virtual void HScroll(int delta);
	virtual void VScroll(int delta);
	virtual bool MouseMove(float x, float y);

	virtual void OnMouseOver();
	virtual void OnMouseOut();

	virtual void CharTyped(wchar_t character);

	// Explicit Positions
	Math::vector4f ParentRelativePivot;	// Pivot Position relative to parent's pivot point.

	Math::vector4f Position;		// Pivot Position relative to anchor if both anchors are in the same position.

	Math::vector4f Size;			// The size of this element not taking into account the element's scale.
	Math::vector4f MinOffset;
	Math::vector4f MaxOffset;
	Math::vector4f MinAnchor;		// Top left anchor's ratio position relative to parent's top left corner.
	Math::vector4f MaxAnchor;		// Bottom right anchor's ratio position relative to parent's top left corner.
	Math::vector4f Pivot;		// Ratio position of the pivot point relative to this element's top left corner.
	Math::vector4f Scale;		// Scale of the element (1.0, 1.0 for unscaled element).

	// Calculated Corner Positions Relative to the Parent's Top Left Corner
	Math::vector4f MinCorner;
	Math::vector4f MaxCorner;

	// Last Mouse Position
	Math::vector4f MousePosition;

	// Transform Matrices
	Math::matrix4f LocalToWorld;
	Math::matrix4f LocalToParent;
	Math::matrix4f ParentToLocal;
	Math::matrix4f WorldToLocal;

	// Misc Properties
	float Rotation;
	bool Active;
	bool HasMouseOver;
	bool Passive;
	bool HasFocus;

	std::vector<std::shared_ptr<UIElement>> Children;
	std::vector<std::shared_ptr<UIElement>> ActiveChildren;
	std::vector<std::shared_ptr<UIElement>> NonPassiveChildren;
	std::weak_ptr<UIElement> Parent;
	UISystem *Window;

	size_t FocusElementIndex = SIZE_MAX;
	size_t MouseOverChildIndex = SIZE_MAX;

private:
	static void _UpdateNonPassiveList(std::shared_ptr<UIElement> pThis, std::shared_ptr<UIElement> removedElement);

	void _RebuildNonPassiveList();
	void _MakeElementPassive(std::shared_ptr<UIElement> element);

	void _UpdateActiveList(std::shared_ptr<UIElement> element);

	void _DeFocus();
	void _UpdateChildren(bool noMouseUpdate);

	void _UnMouseChildren();
	void _UnMouseChild();

	struct vs_pixel_transform
	{
		Math::vector4f Size;
		Math::vector4f Pivot;
	};

};

template<typename T>
inline std::shared_ptr<T> CreateElement(UISystem *system, std::shared_ptr<UIElement> parent)
{
	static_assert(std::is_base_of<UIElement, T>::value, "T must derive from UIElement");
	std::shared_ptr<T> result = std::make_shared<T>(system);
	result->SetParent(parent);
	return result;
}

}}
