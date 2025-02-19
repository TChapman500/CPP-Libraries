#include "UISystem.h"
#include "UIElement.h"
#include "../Graphics/Graphics_Core.h"
#include <iostream>
#include <sstream>
#include <fstream>

using std::shared_ptr;
using namespace TChapman500::Graphics;

namespace TChapman500 {
namespace GUI {

shared_ptr<VertexShader> UISystem::DebugVertexShader;
shared_ptr<VertexShader> UISystem::RectVertexShader;
shared_ptr<VertexShader> UISystem::AdvancedRectVertexShader;
shared_ptr<PixelShader> UISystem::PixelShader;

shared_ptr<ConstantBuffer> UISystem::MatrixBuffer;
shared_ptr<ConstantBuffer> UISystem::UIElementBuffer;
shared_ptr<ConstantBuffer> UISystem::MaterialBuffer;

shared_ptr<VertexBuffer> UISystem::_DebugVtxBuffer;
shared_ptr<IndexBuffer> UISystem::_DebugIdxBuffer;

shared_ptr<InputLayout> UISystem::_DebugIL;
shared_ptr<InputLayout> UISystem::_AdvancedRectIL;

shared_ptr<BlendState> UISystem::_OpaqueBlend;
shared_ptr<BlendState> UISystem::_TransparentBlend;

shared_ptr<Sampler> UISystem::_PointFilter;
shared_ptr<Sampler> UISystem::_LinearFilter;

shared_ptr<DepthStencilState> UISystem::_DepthState;
bool UISystem::DebugMode = false;


void UISystem::Initialize(std::wstring shaderPath)
{
	MatrixBuffer = std::make_shared<Graphics::ConstantBuffer>(8, nullptr, Graphics::ConstantBuffer::buffer_shader::VertexShader);
	UIElementBuffer = std::make_shared<Graphics::ConstantBuffer>(2, nullptr, Graphics::ConstantBuffer::buffer_shader::VertexShader);
	MaterialBuffer = std::make_shared<Graphics::ConstantBuffer>(2, nullptr, Graphics::ConstantBuffer::buffer_shader::PixelShader);

	_PointFilter = std::make_shared<Graphics::Sampler>(Graphics::sampler_mode::Clamp, Graphics::sampler_mode::Clamp, Graphics::sampler_filter::Point, 0.0f);
	_LinearFilter = std::make_shared<Graphics::Sampler>(Graphics::sampler_mode::Clamp, Graphics::sampler_mode::Clamp, Graphics::sampler_filter::Bilinear, 0.0f);

	Graphics::shader_create_params shaderParams;
	shaderParams.BufferRows = { 8, 2 };
	shaderParams.Buffers = { MatrixBuffer, UIElementBuffer };
	shaderParams.Path = shaderPath + L"Vertex.vs";
	shaderParams.Entry = "debug_main";
	DebugVertexShader = std::make_shared<Graphics::VertexShader>(shaderParams);

	shaderParams.Entry = "rect_main";
	RectVertexShader = std::make_shared<Graphics::VertexShader>(shaderParams);

	shaderParams.Entry = "advanced_rect_main";
	AdvancedRectVertexShader = std::make_shared<Graphics::VertexShader>(shaderParams);

	shaderParams.BufferRows = { 2 };
	shaderParams.Buffers = { MaterialBuffer };
	shaderParams.Path = shaderPath + L"Pixel.ps";
	shaderParams.Entry = "main";
	PixelShader = std::make_shared<Graphics::PixelShader>(shaderParams);

	ui_vertex vertices[] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
	};
	unsigned indices[] = { 0, 1, 2, 3, 0 };
	_DebugVtxBuffer = std::make_shared<Graphics::VertexBuffer>(2, 4, (float *)vertices, true);
	_DebugIdxBuffer = std::make_shared<Graphics::IndexBuffer>(5, indices, true);

	Graphics::il_create_param ilDesc[3];
	ilDesc[0].FloatCount = 2;
	ilDesc[0].InputSlot = 0;
	ilDesc[0].IsInstanced = false;
	ilDesc[0].InstanceStepRate = 0;
	ilDesc[0].Semantic = (char *)"POSITION";
	ilDesc[0].SemanticIndex = 0;
	_DebugIL = std::make_shared<Graphics::InputLayout>(1, ilDesc, *DebugVertexShader.get());
	DebugVertexShader->SetInputLayout(_DebugIL);

	ilDesc[1].FloatCount = 4;
	ilDesc[1].InputSlot = 0;
	ilDesc[1].IsInstanced = false;
	ilDesc[1].InstanceStepRate = 0;
	ilDesc[1].Semantic = (char *)"COLOR";
	ilDesc[1].SemanticIndex = 0;

	ilDesc[2].FloatCount = 2;
	ilDesc[2].InputSlot = 0;
	ilDesc[2].IsInstanced = false;
	ilDesc[2].InstanceStepRate = 0;
	ilDesc[2].Semantic = (char *)"TEXCOORD";
	ilDesc[2].SemanticIndex = 0;
	_AdvancedRectIL = std::make_shared<Graphics::InputLayout>(3, ilDesc, *AdvancedRectVertexShader.get());
	AdvancedRectVertexShader->SetInputLayout(_AdvancedRectIL);
	_TransparentBlend = std::make_shared<Graphics::BlendState>(true);
	_DepthState = std::make_shared<Graphics::DepthStencilState>();
}

void UISystem::CleanUp()
{
	MatrixBuffer = nullptr;
	UIElementBuffer = nullptr;
	MaterialBuffer = nullptr;
	DebugVertexShader = nullptr;
	RectVertexShader = nullptr;
	AdvancedRectVertexShader = nullptr;
	PixelShader = nullptr;

	_DebugVtxBuffer = nullptr;
	_DebugIdxBuffer = nullptr;
	_DebugIL = nullptr;
	_AdvancedRectIL = nullptr;
	_OpaqueBlend = nullptr;
	_TransparentBlend = nullptr;

	_PointFilter = nullptr;
	_LinearFilter = nullptr;

	_DepthState = nullptr;
}


UISystem::UISystem(int width, int height)
{
	MouseOverElementIndex = SIZE_MAX;
	MouseX = -1;
	MouseY = -1;
	WindowX = 0;
	WindowY = 0;

	WindowX = width;
	WindowY = height;

	WorldToScreen = Math::matrix4f(
		2.0f / (float)width, 0.0f, 0.0f, -1.0f,
		0.0f, -2.0f / (float)height, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

UISystem::~UISystem()
{

}

void UISystem::OnKeyPressed(Input::key_code key)
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->OnKeyPressed(key);
}

void UISystem::OnKeyReleased(Input::key_code key)
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->OnKeyReleased(key);
}

void UISystem::LButtonDown()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->LButtonDown();
}

void UISystem::LButtonUp()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->LButtonUp();
}

void UISystem::MButtonDown()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->MButtonDown();
}

void UISystem::MButtonUp()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->MButtonUp();
}

void UISystem::RButtonDown()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->RButtonDown();
}

void UISystem::RButtonUp()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->RButtonUp();
}

void UISystem::X1ButtonDown()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->X1ButtonDown();
}

void UISystem::X1ButtonUp()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->X1ButtonUp();
}

void UISystem::X2ButtonDown()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->X2ButtonDown();
}

void UISystem::X2ButtonUp()
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->X2ButtonUp();
}

bool UISystem::AddElement(const std::shared_ptr<UIElement> newElement)
{
	if (!newElement) return false;

	// Find the root element.
	std::shared_ptr<UIElement> root = newElement;
	while (!root->Parent.expired())
		root = root->Parent.lock();

	// Make sure the element does not already exist.
	for (size_t i = 0; i < Elements.size(); i++)
	{
		// It already exists
		if (root == Elements[i])
			return false;
	}

	Elements.push_back(newElement);

	if (!newElement->Active) return true;
	ActiveElements.push_back(newElement);

	if (newElement->Passive) return true;
	NonPassiveElements.push_back(newElement);
	if (HasMouseOver) MouseMove(MouseX, MouseY);
	return true;
}

size_t UISystem::GetElementIndex(std::shared_ptr<UIElement> element)
{
	for (size_t i = 0; i < Elements.size(); i++)
	{
		if (Elements[i] == element)
			return i;
	}
	return SIZE_MAX;
}

size_t UISystem::GetElementCount() { return Elements.size(); }


std::shared_ptr<UIElement> UISystem::GetElement(size_t index)
{
	if (index >= Elements.size()) return nullptr;
	return Elements[index];
}

bool UISystem::MoveElement(size_t from, size_t to)
{
	if (from >= Elements.size() || from == to) return false;

	std::shared_ptr<UIElement> element = Elements[from];

	// Moving Down the List
	if (from > to)
	{
		for (size_t i = from; i > to; i--)
			Elements[i] = Elements[i - 1];

		Elements[to] = element;
	}
	// Moving Up the List
	else
	{
		if (to >= Elements.size()) to = Elements.size() - 1;
		for (size_t i = from; i < to; i++)
			Elements[i] = Elements[i + 1];

		Elements[to] = element;
	}

	if (!element->Active) return true;

	// Rebuild Active list
	ActiveElements.clear();
	for (const shared_ptr<UIElement> &child : Elements)
	{
		if (child->Active)
			ActiveElements.push_back(child);
	}

	if (element->Passive) return true;

	// Rebuild Passive List
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverElementIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveElements[MouseOverElementIndex];
		MouseOverElementIndex = SIZE_MAX;
	}
	NonPassiveElements.clear();
	for (size_t i = 0; i < ActiveElements.size(); i++)
	{
		const shared_ptr<UIElement> &child = ActiveElements[i];
		if (!child->Passive) NonPassiveElements.push_back(child);
		if (child == mouseOverElement) MouseOverElementIndex = i;
	}
	if (HasMouseOver) MouseMove(MouseX, MouseY);
	return true;
}

bool UISystem::RemoveElement(size_t index)
{
	if (index >= Elements.size()) return false;
	std::shared_ptr<UIElement> elementToRemove = Elements[index];
	Elements.erase(Elements.begin() + index);

	// Remove from active list if necessary
	if (!elementToRemove->Active) return true;
	for (size_t i = 0; i < ActiveElements.size(); i++)
	{
		if (ActiveElements[i] == elementToRemove)
		{
			ActiveElements.erase(ActiveElements.begin() + i);
			break;
		}
	}

	// Remove from non-passive list if necessary
	if (elementToRemove->Passive) return true;
	
	// Doesn't actually make elements passive, just removes them from the non-passive list
	MakeElementPassive(elementToRemove);
	return true;
}

void UISystem::HScroll(int delta)
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->HScroll(delta);
}

void UISystem::VScroll(int delta)
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->VScroll(delta);
}

void UISystem::MouseMove(int x, int y)
{
	HasMouseOver = true;

	MouseX = x;
	MouseY = y;
	
	size_t max = NonPassiveElements.size();
	for (size_t i = 0; i < max; i++)
	{
		size_t c = max - (i + 1);
		// Stop at the first element.
		if (NonPassiveElements[c]->MouseMove((float)MouseX, (float)MouseY))
		{
			// Make sure to prevent double-fires of the MouseOut() event
			if (MouseOverElementIndex < c)
				_UnMouseElement(NonPassiveElements[MouseOverElementIndex]);

			MouseOverElementIndex = c;
			return;
		}
	}

	if (MouseOverElementIndex != SIZE_MAX)
		_UnMouseElement(NonPassiveElements[MouseOverElementIndex]);

	MouseOverElementIndex = SIZE_MAX;

}

void UISystem::MouseOut()
{
	HasMouseOver = false;
	if (MouseOverElementIndex != SIZE_MAX)
		_UnMouseElement(NonPassiveElements[MouseOverElementIndex]);
	MouseOverElementIndex = SIZE_MAX;
}

void UISystem::GetMousePosition(int &x, int &y)
{
	x = MouseX;
	y = MouseY;
}

void UISystem::SetWindowSize(int x, int y)
{
	WindowX = x;
	WindowY = y;


	WorldToScreen = Math::matrix4f(
		2.0f / (float)x, 0.0f, 0.0f, -1.0f,
		0.0f, -2.0f / (float)y, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	for (size_t i = 0; i < Elements.size(); i++)
	{
		if (!Elements[i]->SetPosition(Elements[i]->Position.X, Elements[i]->Position.Y))
			Elements[i]->SetOffsets(Elements[i]->MinOffset.X, Elements[i]->MinOffset.Y, Elements[i]->MaxOffset.X, Elements[i]->MaxOffset.Y);
	}
	MouseMove(MouseX, MouseY);
}

void UISystem::GetWindowSize(int &x, int &y)
{
	x = WindowX;
	y = WindowY;
}

void UISystem::CharTyped(wchar_t character)
{
	for (const std::shared_ptr<UIElement> &element : NonPassiveElements)
		element->CharTyped(character);
}

void UISystem::RenderWireframe(std::shared_ptr<UIElement> &element)
{
	matrix_buffer matrixData = { element->LocalToWorld, WorldToScreen };
	MatrixBuffer->WriteBuffer(&matrixData);
	element_buffer elementData = { element->Pivot, element->Size };
	UIElementBuffer->WriteBuffer(&elementData);

	if (element->HasMouseOver)
	{
		material_buffer materialData = { { 1.0f, 0.0f, 0.0f, 1.0f }, 0 };
		MaterialBuffer->WriteBuffer(&materialData);
	}
	else
	{
		material_buffer materialData = { { 1.0f, 1.0f, 1.0f, 1.0f }, 0 };
		MaterialBuffer->WriteBuffer(&materialData);
	}

	Graphics::DrawIndexed(5);

	for (size_t i = 0; i < element->ActiveChildren.size(); i++)
		RenderWireframe(element->ActiveChildren[i]);
}

void UISystem::Render(float deltaTime)
{
	_DepthState->Set();
	PixelShader->Set();

	// Render elements
	for (size_t i = 0; i < ActiveElements.size(); i++)
		ActiveElements[i]->Render(deltaTime);

	if (DebugMode)
	{
		DebugVertexShader->Set();

		_DebugIL->Set();

		_DebugVtxBuffer->Set(0);
		_DebugIdxBuffer->Set(0);

		Graphics::SetTopology(Graphics::topology::LineStrip);

		// Render wireframe
		for (size_t i = 0; i < ActiveElements.size(); i++)
			RenderWireframe(ActiveElements[i]);
	}
}

void UISystem::_UnMouseElement(std::shared_ptr<UIElement> &element)
{
	element->HasMouseOver = false;
	element->OnMouseOut();

	if (element->MouseOverChildIndex == SIZE_MAX) return;
	std::shared_ptr<UIElement> currElement = element->NonPassiveChildren[element->MouseOverChildIndex];
	element->MouseOverChildIndex = SIZE_MAX;
	size_t currElementIndex;

	while (true)
	{
		currElement->HasMouseOver = false;
		currElement->OnMouseOut();

		// Clear the element index
		currElementIndex = currElement->MouseOverChildIndex;
		if (currElementIndex == SIZE_MAX) break;
		currElement->MouseOverChildIndex = SIZE_MAX;

		// Go to the next child that had the mouse
		currElement = currElement->NonPassiveChildren[currElementIndex];
	}
}

void UISystem::UpdateActiveList(std::shared_ptr<UIElement> element)
{
	if (!element->Active)
	{
		// Remove from active list.
		for (size_t i = 0; i < ActiveElements.size(); i++)
		{
			if (ActiveElements[i] == element)
			{
				ActiveElements.erase(ActiveElements.begin() + i);
				break;
			}
		}
		// No further action needed
		if (element->Passive) return;

		MakeElementPassive(element);
		return;
	}

	// Rebuild active list
	ActiveElements.clear();
	for (const shared_ptr<UIElement> &child : Elements)
	{
		if (child->Active)
			ActiveElements.push_back(child);
	}
	// No further action is needed for passive elements
	if (element->Passive) return;

	RebuildNonPassiveList();
}

void UISystem::UpdatePassiveList(std::shared_ptr<UIElement> element)
{
}

void UISystem::RebuildNonPassiveList()
{
	// This function always adds to the list or changes the order

	// Cache mouse over element
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverElementIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveElements[MouseOverElementIndex];
		MouseOverElementIndex = SIZE_MAX;
	}

	// Rebuild the list
	NonPassiveElements.clear();
	for (size_t i = 0; i < ActiveElements.size(); i++)
	{
		// Skip passive elements
		if (ActiveElements[i]->Passive) continue;

		// This element has the mouse over it
		if (ActiveElements[i] == mouseOverElement)
			MouseOverElementIndex = i;

		// Add the element to the non-passive list
		NonPassiveElements.push_back(ActiveElements[i]);
	}

	// Ensure that the correct element has the mouseover
	if (HasMouseOver) MouseMove(MouseX, MouseY);
}

void UISystem::MakeElementPassive(std::shared_ptr<UIElement> element)
{
	// Store mouse over element
	shared_ptr<UIElement> mouseOverElement;
	if (MouseOverElementIndex != SIZE_MAX)
	{
		mouseOverElement = NonPassiveElements[MouseOverElementIndex];
		// Element going passive has the mouse over it
		if (mouseOverElement == element)
		{
			_UnMouseElement(element);
			mouseOverElement = nullptr;
		}
		MouseOverElementIndex = SIZE_MAX;
	}

	// Remove element from the non-passive list
	for (size_t i = 0; i < NonPassiveElements.size(); i++)
	{
		if (NonPassiveElements[i] == element)
		{
			NonPassiveElements.erase(NonPassiveElements.begin() + i);
			break;
		}
	}

	// Restore mouse over element index
	if (mouseOverElement)
	{
		for (size_t i = 0; i < NonPassiveElements.size(); i++)
		{
			if (NonPassiveElements[i] == mouseOverElement)
			{
				MouseOverElementIndex = i;
				break;
			}
		}
	}

	// Recalculate mouse over element
	if (HasMouseOver) MouseMove(MouseX, MouseY);
}


}}
