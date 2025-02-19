#include "UIElement.h"
#include <emmintrin.h>
#include "UISystem.h"
#include <sstream>

using std::shared_ptr;

namespace TChapman500 {
namespace GUI {

using namespace Math;
UIElement::UIElement(UISystem *uiSystem)
{
	MouseOverChildIndex = SIZE_MAX;
	HasMouseOver = false;
	Window = uiSystem;
	Active = true;
	Passive = true;
	HasFocus = false;
	Rotation = 0.0f;

	Size = vector4f(10.0f, 10.0f);
	Scale = vector4f(1.0f, 1.0f);
	Pivot = vector4f(0.5f, 0.5f);
	MinAnchor = vector4f(0.5f, 0.5f);
	MaxAnchor = vector4f(0.5f, 0.5f);
	UIElement::SetPosition(0.0f, 0.0f);
}

UIElement::~UIElement()
{
}

void UIElement::Render(float deltaTime)
{
	for (size_t i = 0; i < ActiveChildren.size(); i++)
		ActiveChildren[i]->Render(deltaTime);
}

bool UIElement::SetPosition(float x, float y)
{
	// We can only do this when the anchors are in the same spot.
	// Otherwise, we should move the corners of the element.
	if (MinAnchor != MaxAnchor) return false;

	Position.X = x;
	Position.Y = y;

	if (Parent.expired())
	{
		int wndX, wndY;
		Window->GetWindowSize(wndX, wndY);
		vector4f wndSize((float)wndX, (float)wndY);

		// No parent means element is relative to the window
		ParentRelativePivot = (wndSize * MinAnchor) + Position;
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();
		vector4f anchorPos = parent->Size * (MinAnchor - parent->Pivot);
		ParentRelativePivot = anchorPos + Position;
	}

	MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
	MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);

	MinOffset = Position - (Pivot * Size);
	MaxOffset = Position + ((vector4f(1.0f, 1.0f) - Pivot) * Size);

	_UpdateChildren(false);
	return true;
}

bool UIElement::SetSize(float x, float y)
{
	// We can only do this when the anchors are in the same spot.
	// Otherwise, we should move the corners of the element.
	if (MinAnchor != MaxAnchor) return false;

	Size.X = x;
	Size.Y = y;

	MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
	MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);

	MinOffset = Position - (Pivot * Size);
	MaxOffset = Position + ((vector4f(1.0f, 1.0f) - Pivot) * Size);

	_UpdateChildren(false);
	return true;
}

bool UIElement::SetAnchors(float minX, float minY, float maxX, float maxY)
{
	vector4f minAnchor(minX, minY);
	vector4f maxAnchor(maxX, maxY);

	if (minAnchor == maxAnchor)
	{
		// Going from min and max offset to position and size
		if (MinAnchor != MaxAnchor)
		{
			vector4f minPosition;
			vector4f maxPosition;

			if (Parent.expired())
			{
				int wndX, wndY;
				Window->GetWindowSize(wndX, wndY);
				vector4f wndSize((float)wndX, (float)wndY);

				minPosition = (MinAnchor * wndSize) + MinOffset;
				maxPosition = (MaxAnchor * wndSize) + MaxOffset;
			}
			else
			{
				std::shared_ptr<UIElement> parent = Parent.lock();

				minPosition = (MinAnchor * parent->Size) + MinOffset;
				maxPosition = (MaxAnchor * parent->Size) + MaxOffset;
			}

			Size = maxPosition - minPosition;
		}

		MinAnchor = minAnchor;
		MaxAnchor = maxAnchor;

		if (Parent.expired())
		{
			int wndX, wndY;
			Window->GetWindowSize(wndX, wndY);
			vector4f wndSize((float)wndX, (float)wndY);

			vector4f anchorPos = wndSize * MinAnchor;
			Position = anchorPos - ParentRelativePivot;
		}
		else
		{
			std::shared_ptr<UIElement> parent = Parent.lock();

			vector4f anchorPos = parent->Size * MinAnchor;
			Position = anchorPos - ParentRelativePivot;
		}

		MinOffset = Position - (Pivot * Size);
		MaxOffset = Position + ((vector4f(1.0f, 1.0f) - Pivot) * Size);

		_UpdateChildren(false);
		return true;
	}

	if (Parent.expired())
	{
		int wndX, wndY;
		Window->GetWindowSize(wndX, wndY);
		vector4f wndSize((float)wndX, (float)wndY);

		MinOffset = Position - (Pivot * Size) - (minAnchor * wndSize);
		MaxOffset = Position + ((vector4f(1.0f, 1.0f) - Pivot) * Size) - (maxAnchor * wndSize);
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();

		MinOffset = Position - (Pivot * Size) - (minAnchor * parent->Size);
		MaxOffset = Position + ((vector4f(1.0f, 1.0f) - Pivot) * Size) - (maxAnchor * parent->Size);
	}

	MinAnchor = minAnchor;
	MaxAnchor = maxAnchor;

	_UpdateChildren(false);
	return true;
}

bool UIElement::SetOffsets(float minX, float minY, float maxX, float maxY)
{
	if (MinAnchor == MaxAnchor) return false;

	vector4f minOffset(minX, minY);
	vector4f maxOffset(maxX, maxY);

	MinOffset = minOffset;
	MaxOffset = maxOffset;

	vector4f minAnchorPos;
	vector4f maxAnchorPos;

	if (Parent.expired())
	{
		int wndX, wndY;
		Window->GetWindowSize(wndX, wndY);
		vector4f wndSize((float)wndX, (float)wndY);

		minAnchorPos = MinAnchor * wndSize;
		maxAnchorPos = MaxAnchor * wndSize;
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();

		minAnchorPos = (MinAnchor - parent->Pivot) * parent->Size;
		maxAnchorPos = (MaxAnchor - parent->Pivot) * parent->Size;
	}

	vector4f minPosition = minOffset + minAnchorPos;
	vector4f maxPosition = maxOffset + maxAnchorPos;

	Size = maxPosition - minPosition;
	
	ParentRelativePivot = minPosition + (Size * Pivot);

	MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
	MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);

	_UpdateChildren(false);
	return true;
}

bool UIElement::SetPivot(float x, float y)
{
	Pivot.X = x;
	Pivot.Y = y;

	// Use position and size
	if (MinAnchor == MaxAnchor)
	{
		Position = MinOffset + (Pivot * Size);

		if (Parent.expired())
		{
			int wndX, wndY;
			Window->GetWindowSize(wndX, wndY);
			vector4f wndSize((float)wndX, (float)wndY);

			// No parent means element is relative to the window
			ParentRelativePivot = Position + (wndSize * MinAnchor);
		}
		else
		{
			std::shared_ptr<UIElement> parent = Parent.lock();
			vector4f anchorPos = parent->Size * (MinAnchor - parent->Pivot);
			ParentRelativePivot = Position + anchorPos;
		}

		MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
		MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);

		_UpdateChildren(false);
		return true;
	}

	vector4f minPosition;
	vector4f maxPosition;
	vector4f parentSize;

	if (Parent.expired())
	{
		int wndX, wndY;
		Window->GetWindowSize(wndX, wndY);
		vector4f wndSize((float)wndX, (float)wndY);
		parentSize = wndSize;
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();
		parentSize = parent->Size;
	}

	minPosition = parentSize * MinAnchor + MinOffset;
	maxPosition = parentSize * MaxAnchor + MaxOffset;

	ParentRelativePivot = minPosition + ((maxPosition - minPosition) * Pivot);
	MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
	MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);
	
	_UpdateChildren(false);
	return true;
}

bool UIElement::SetScale(float x, float y)
{
	Scale.X = x;
	Scale.Y = y;
	MinCorner = ParentRelativePivot - (Pivot * Size * Scale);
	MaxCorner = ParentRelativePivot + ((vector4f(1.0f, 1.0f) - Pivot) * Size * Scale);

	_UpdateChildren(false);
	return true;
}

bool UIElement::SetRotation(float angle)
{
	Rotation = angle;
	_UpdateChildren(false);
	return true;
}

bool UIElement::IsValidAsChildObject(const std::shared_ptr<UIElement> &child)
{
	// Nice try!
	if (!child || child.get() == this) return false;

	// Verify that this isn't a parent element
	std::shared_ptr<UIElement> currParent = Parent.lock();

	// Go until we reach the Root Element
	while (currParent)
	{
		// Not a parent element
		if (currParent != child)
		{
			currParent = currParent->Parent.lock();
			continue;
		}

		// Cannot add this element because it is a parent element
		return false;
	}

	// Verify that the element is not already a child element
	for (size_t i = 0; i < Children.size(); i++)
	{
		if (Children[i] == child)
			return false;
	}

	// All clear
	return true;
}

bool UIElement::AddChild(const std::shared_ptr<UIElement> &child)
{
	// Add child to the list
	Children.push_back(child);

	// Update child's relative positioning.
	if (!child->SetPosition(child->Position.X, child->Position.Y))
		child->SetOffsets(child->MinOffset.X, child->MinOffset.Y, child->MaxOffset.X, child->MaxOffset.Y);

	// Add active child to the list
	if (child->Active) ActiveChildren.push_back(child);
	if (child->Passive) return true;

	// We can just append this to the list
	NonPassiveChildren.push_back(child);

	// Do nothing more if this element is not active
	if (Active)
	{
		// Element is currently passive
		if (Passive)
		{
			// Clear the passive flag
			Passive = false;

			// Update the UI System
			if (Parent.expired()) Window->RebuildNonPassiveList();
			// Update the parent
			else Parent.lock()->_RebuildNonPassiveList();
		}
		// Element is non-passive
		else if (HasMouseOver) MouseMove(MousePosition.X, MousePosition.Y);
	}

	return true;
}

bool UIElement::RemoveChild(size_t index)
{
	// Out of bounds
	if (index >= Children.size()) return false;

	// Remove from child list
	shared_ptr<UIElement> child = Children[index];
	Children.erase(Children.begin() + index);

	// Remove from active list
	for (size_t i = 0; i < ActiveChildren.size(); i++)
	{
		if (ActiveChildren[i] == child)
		{
			ActiveChildren.erase(ActiveChildren.begin() + i);
			break;
		}
	}

	// Somehow, this function is still appropriate here
	_MakeElementPassive(child);
	return true;
}

std::shared_ptr<UIElement> UIElement::GetChild(size_t index)
{
	if (index >= Children.size()) return nullptr;
	return Children[index];
}

size_t UIElement::GetChildIndex(const std::shared_ptr<UIElement> &child)
{
	for (size_t i = 0; i < Children.size(); i++)
	{
		if (Children[i] == child)
			return i;
	}

	return SIZE_MAX;
}

bool UIElement::MoveChild(size_t from, size_t to)
{
	// Out of bounds or no movement
	if (from >= Children.size() || from == to) return false;

	std::shared_ptr<UIElement> element = Children[from];

	// Moving Down the List
	if (from > to)
	{
		for (size_t i = from; i > to; i--) Children[i] = Children[i - 1];
		Children[to] = element;
	}
	// Moving Up the List
	else
	{
		if (to >= Children.size()) to = Children.size() - 1;
		for (size_t i = from; i < to; i++) Children[i] = Children[i + 1];
		Children[to] = element;
	}

	// Rebuild Active list
	ActiveChildren.clear();
	for (const shared_ptr<UIElement> &child : Children)
	{
		if (child->Active)
			ActiveChildren.push_back(child);
	}

	// Rebuild Passive List
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverChildIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveChildren[MouseOverChildIndex];
		MouseOverChildIndex = SIZE_MAX;
	}
	NonPassiveChildren.clear();
	for (size_t i = 0; i < ActiveChildren.size(); i++)
	{
		const shared_ptr<UIElement> &child = ActiveChildren[i];
		if (!child->Passive) NonPassiveChildren.push_back(child);
		if (child == mouseOverElement) MouseOverChildIndex = i;
	}
	if (HasMouseOver) MouseMove(MousePosition.X, MousePosition.Y);
	return true;
}

size_t UIElement::GetChildCount() { return Children.size(); }

void UIElement::SetActive(bool active)
{
	// No state change
	if (Active == active) return;
	// Update active flag
	Active = active;

	// Update parent state
	if (Parent.expired()) Window->UpdateActiveList(shared_from_this());
	else Parent.lock()->_UpdateActiveList(shared_from_this());
}

bool UIElement::GetPassive() { return true; }

std::weak_ptr<UIElement> UIElement::GetParent() { return Parent; }

bool UIElement::SetParent(const std::shared_ptr<UIElement> &parent)
{
	// We can't get parented to the target object or no change in parent element
	if (parent && (parent == Parent.lock() || !parent->IsValidAsChildObject(shared_from_this()))) return false;

	// Remove ourselves from the current child list.
	if (Parent.expired()) Window->RemoveElement(Window->GetElementIndex(shared_from_this()));
	else
	{
		std::shared_ptr<UIElement> currParent = Parent.lock();
		currParent->RemoveChild(currParent->GetChildIndex(shared_from_this()));
	}

	// Change to the new parent.
	Parent = parent;
	if (parent) parent->AddChild(shared_from_this());
	else Window->AddElement(shared_from_this());
	return true;
}

void UIElement::OnKeyPressed(Input::key_code key)
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->OnKeyPressed(key);
}

void UIElement::OnKeyReleased(Input::key_code key)
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->OnKeyReleased(key);
}

void UIElement::LButtonDown()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->LButtonDown();
}

void UIElement::LButtonUp()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->LButtonUp();
}

void UIElement::MButtonDown()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->MButtonDown();
}

void UIElement::MButtonUp()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->MButtonUp();
}

void UIElement::RButtonDown()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->RButtonDown();
}

void UIElement::RButtonUp()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->RButtonUp();
}

void UIElement::X1ButtonDown()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->X1ButtonDown();
}

void UIElement::X1ButtonUp()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->X1ButtonUp();
}

void UIElement::X2ButtonDown()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->X2ButtonDown();
}

void UIElement::X2ButtonUp()
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->X2ButtonUp();
}

void UIElement::HScroll(int delta)
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->HScroll(delta);
}

void UIElement::VScroll(int delta)
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->VScroll(delta);
}

bool UIElement::MouseMove(float x, float y)
{
	if (!Active || Passive) return false;

	// Transform mouse position using SSE
	MousePosition = { x, y , 0.0f, 0.0f };
	vector4f mousePos = { x, y, 0.0f, 1.0f };
	mousePos = mousePos * ParentToLocal;
	mousePos.W = 0.0f;

	vector4f one = { 1.0f, 1.0f, 0.0f, 0.0f };

	// Bounds Check
	__m128 result = _mm_or_ps(
		_mm_cmplt_ps(mousePos.Vector, (-(Pivot * Size)).Vector),
		_mm_cmpgt_ps(mousePos.Vector, ((one - Pivot) * Size).Vector));
	int mask = _mm_movemask_ps(result);
	
	// Mouse is outside of the bounds.
	if (mask)
	{
		// Do mouse out event.
		if (HasMouseOver)
		{
			HasMouseOver = false;
			OnMouseOut();
			_UnMouseChild();
		}
		return false;
	}

	if (!HasMouseOver)
	{
		HasMouseOver = true;
		OnMouseOver();
	}

	size_t max = NonPassiveChildren.size();
	for (size_t i = 0; i < max; i++)
	{
		size_t c = max - (i + 1);
		// Stop at the first element that the mouse is over.
		if (NonPassiveChildren[c]->MouseMove(mousePos.X, mousePos.Y))
		{
			// Make sure that we don't accidentally trigger multiple MouseOut() events.
			if (MouseOverChildIndex != c && MouseOverChildIndex != SIZE_MAX) _UnMouseChild();
			MouseOverChildIndex = c;
			return true;
		}
	}

	// Mouse isn't over any child elements.
	_UnMouseChild();
	return true;
}

void UIElement::OnMouseOver()
{
}

void UIElement::OnMouseOut()
{
}

void UIElement::CharTyped(wchar_t character)
{
	for (const std::shared_ptr<UIElement> &child : NonPassiveChildren)
		child->CharTyped(character);
}

void UIElement::_UpdateNonPassiveList(shared_ptr<UIElement> pThis, shared_ptr<UIElement> removedElement)
{
	bool propagate = true;
	bool recalcMouseOver = false;
	while (propagate)
	{
		// This will be reset if we need to propagate further
		propagate = false;

		// Save the element that the mouse is currently over
		shared_ptr<UIElement> mouseOverElement;
		if (pThis->MouseOverChildIndex != SIZE_MAX)
		{
			mouseOverElement = pThis->NonPassiveChildren[pThis->MouseOverChildIndex];
			pThis->MouseOverChildIndex = SIZE_MAX;
		}

		// Simply remove the element from the list
		if (removedElement)
		{
			// Search for the element to remove
			for (size_t i = 0; i < pThis->NonPassiveChildren.size(); i++)
			{
				shared_ptr<UIElement> &child = pThis->ActiveChildren[i];
				// Found the element to remove
				if (child == removedElement)
				{
					// The element to remove has the mouse over it
					if (child == mouseOverElement)
					{
						mouseOverElement = nullptr;
						recalcMouseOver = true;
						pThis->MouseOverChildIndex = i;
					}
					// Propagate mouse out all the way up the tree
					pThis->_UnMouseChild();
					pThis->NonPassiveChildren.erase(pThis->NonPassiveChildren.begin() + i);
					continue;
				}
				// Update the mouse over index
				if (child == mouseOverElement) pThis->MouseOverChildIndex = i;
			}

			// There's nothing left
			if (pThis->NonPassiveChildren.empty())
			{
				pThis->Passive = pThis->GetPassive();
				if (pThis->Passive)
				{
					pThis->OnMouseOut();
					pThis->MouseOverChildIndex = SIZE_MAX;
					propagate = true;
				}
			}
		}
		// Add the element to the list
		else
		{
			// Always recalculate the mouse over element when adding a non-passive element to the list
			recalcMouseOver = true;

			// Going non-passive
			if (pThis->NonPassiveChildren.empty())
			{
				pThis->Passive = false;
				propagate = true;
			}

			// Rebuild the entire non-passive list
			pThis->NonPassiveChildren.clear();
			for (const shared_ptr<UIElement> &child : pThis->ActiveChildren)
			{
				if (!child->Passive)
				{
					// Add the element to the non-passive list
					pThis->NonPassiveChildren.push_back(child);
					// Update mouse over index if necessary
					if (child == mouseOverElement) pThis->MouseOverChildIndex = pThis->NonPassiveChildren.size() - 1;
				}
			}
		}

		// Current element is the element to remove in the next iteration
		if (propagate)
		{
			if (removedElement) removedElement = pThis;

			// We have reached the root
			if (pThis->Parent.expired())
			{
				pThis->Window->UpdatePassiveList(removedElement);
				break;
			}
			// We have not reached the root
			pThis = pThis->Parent.lock();
		}
		// Not propagating, but we need to recalculate the mouse over element
		else if (!pThis->Passive && recalcMouseOver) pThis->MouseMove(pThis->MousePosition.X, pThis->MousePosition.Y);
	}
}

void UIElement::_RebuildNonPassiveList()
{
	// This function always adds to the list or changes the order

	// Cache mouse over element
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverChildIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveChildren[MouseOverChildIndex];
		MouseOverChildIndex = SIZE_MAX;
	}

	// Rebuild the list
	NonPassiveChildren.clear();
	for (size_t i = 0; i < ActiveChildren.size(); i++)
	{
		// Skip passive elements
		if (ActiveChildren[i]->Passive) continue;

		// This element has the mouse over it
		if (ActiveChildren[i] == mouseOverElement)
			MouseOverChildIndex = i;

		// Add the element to the non-passive list
		NonPassiveChildren.push_back(ActiveChildren[i]);
	}

	// If this element is active, then signal the parent element.
	if (Active)
	{
		// Element is currently passive (parent element needs to update)
		if (Passive)
		{
			// Clear the passive flag
			Passive = false;

			// Update the UI System
			if (Parent.expired()) Window->RebuildNonPassiveList();
			// Update the parent
			else Parent.lock()->_RebuildNonPassiveList();
		}
		// Element is non-passive (parent element does not need to update)
		else if (HasMouseOver) MouseMove(MousePosition.X, MousePosition.Y);
	}
}

void UIElement::_MakeElementPassive(shared_ptr<UIElement> element)
{
	// Store mouse over element
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverChildIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveChildren[MouseOverChildIndex];
		// Element going passive has the mouse over it
		if (mouseOverElement == element)
		{
			_UnMouseChild();
			mouseOverElement = nullptr;
		}
		MouseOverChildIndex = SIZE_MAX;
	}

	// Remove element from the non-passive list
	for (size_t i = 0; i < NonPassiveChildren.size(); i++)
	{
		if (NonPassiveChildren[i] == element)
		{
			NonPassiveChildren.erase(NonPassiveChildren.begin() + i);
			break;
		}
	}

	// Nothing else is left
	if (NonPassiveChildren.empty())
	{
		Passive = GetPassive();
		// This element is always passive
		if (Passive)
		{
			if (HasMouseOver)
			{
				HasMouseOver = false;
				OnMouseOut();
			}

			// Parent element needs to be nodified that we're becoming passive.
			if (Parent.expired()) Window->MakeElementPassive(shared_from_this());
			else Parent.lock()->_MakeElementPassive(shared_from_this());
		}
		return;
	}

	// Restore mouse over element index
	if (mouseOverElement)
	{
		for (size_t i = 0; i < NonPassiveChildren.size(); i++)
		{
			if (NonPassiveChildren[i] == mouseOverElement)
			{
				MouseOverChildIndex = i;
				break;
			}
		}
	}

	// Recalculate mouse over element
	if (HasMouseOver) MouseMove(MousePosition.X, MousePosition.Y);
}

void UIElement::_UpdateActiveList(std::shared_ptr<UIElement> element)
{
	if (!element->Active)
	{
		// Remove from active list.
		for (size_t i = 0; i < ActiveChildren.size(); i++)
		{
			if (ActiveChildren[i] == element)
			{
				ActiveChildren.erase(ActiveChildren.begin() + i);
				break;
			}
		}
		// No further action needed
		if (element->Passive) return;

		_MakeElementPassive(element);
		return;
	}

	// Rebuild active list
	ActiveChildren.clear();
	for (const shared_ptr<UIElement> &child : Children)
	{
		if (child->Active)
			ActiveChildren.push_back(child);
	}
	// No further action is needed for passive elements
	if (element->Passive) return;

	_RebuildNonPassiveList();
}

void UIElement::_DeFocus()
{
	HasFocus = false;
	if (FocusElementIndex < NonPassiveChildren.size())
		NonPassiveChildren[FocusElementIndex]->OnMouseOut();

	FocusElementIndex = SIZE_MAX;
}

void UIElement::_UpdateChildren(bool noMouseUpdate)
{
	matrix4f scaleMatrix = matrix4f::identity();
	scaleMatrix.M11 = Scale.X;
	scaleMatrix.M22 = Scale.Y;

	matrix4f iScaleMatrix = matrix4f::identity();
	iScaleMatrix.M11 = 1.0f / Scale.X;
	iScaleMatrix.M22 = 1.0f / Scale.Y;

	ParentToLocal = (matrix4f::rotateZ(Rotation) * iScaleMatrix) * (matrix4f::translate(matrix4f::identity(), -ParentRelativePivot)).transposed();

	LocalToParent = matrix4f::rotateZ(-Rotation) * scaleMatrix;
	LocalToParent = matrix4f::translate(LocalToParent, ParentRelativePivot);

	if (Parent.expired())
	{
		LocalToWorld = LocalToParent;
		WorldToLocal = ParentToLocal;
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();
		LocalToWorld = parent->LocalToWorld * LocalToParent.transposed();
		WorldToLocal = ParentToLocal * parent->WorldToLocal.transposed();
	}

	for (size_t i = 0; i < Children.size(); i++)
	{
		if (!Children[i]->SetPosition(Children[i]->Position.X, Children[i]->Position.Y))
			Children[i]->SetOffsets(Children[i]->MinOffset.X, Children[i]->MinOffset.Y, Children[i]->MaxOffset.X, Children[i]->MaxOffset.Y);
	}

	if (noMouseUpdate) return;

	if (Parent.expired())
	{
		int mouseX, mouseY;
		Window->GetMousePosition(mouseX, mouseY);
		Window->MouseMove(mouseX, mouseY);
	}
	else
	{
		std::shared_ptr<UIElement> parent = Parent.lock();
		parent->MouseMove(parent->MousePosition.X, parent->MousePosition.Y);
	}
}

void UIElement::_UnMouseChildren()
{
	// Fire MouseOut() event.
	HasMouseOver = false;
	OnMouseOut();

	if (MouseOverChildIndex == SIZE_MAX) return;
	NonPassiveChildren[MouseOverChildIndex]->_UnMouseChildren();
	MouseOverChildIndex = SIZE_MAX;
}

void UIElement::_UnMouseChild()
{
	// Clear mouse over index
	if (MouseOverChildIndex == SIZE_MAX) return;
	std::shared_ptr<UIElement> currElement = NonPassiveChildren[MouseOverChildIndex];
	MouseOverChildIndex = SIZE_MAX;

	size_t currElementIndex;
	while (true)
	{
		// Remove mouse over flag and call mouse out event
		currElement->HasMouseOver = false;
		currElement->OnMouseOut();

		// Cache and clear the element index
		if (currElement->MouseOverChildIndex == SIZE_MAX) break;
		currElementIndex = currElement->MouseOverChildIndex;
		currElement->MouseOverChildIndex = SIZE_MAX;

		// Go to the next child that had the mouse
		currElement = currElement->NonPassiveChildren[currElementIndex];
	}
}

}}
