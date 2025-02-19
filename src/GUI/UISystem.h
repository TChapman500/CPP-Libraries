#pragma once
#include "../InputSystem/input.h"
#include <vector>
#include <memory>
#include <vectors.h>
#include "../Graphics/Buffers.h"
#include "../Graphics/Textures.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/States.h"

namespace TChapman500 {
namespace GUI {

struct matrix_buffer
{
	Math::matrix4f LocalToWorld;
	Math::matrix4f WorldToScreen;
};
struct element_buffer
{
	Math::vector4f Pivot;
	Math::vector4f Size;
};
struct material_buffer
{
	Math::vector4f Color;
	unsigned UseTexture;
	unsigned Padding[3];
};

class UIElement;
class UISystem
{
public:
	UISystem(int width, int height);
	~UISystem();
	
	void OnKeyPressed(Input::key_code key);
	void OnKeyReleased(Input::key_code key);

	void LButtonDown();
	void LButtonUp();
	void MButtonDown();
	void MButtonUp();
	void RButtonDown();
	void RButtonUp();

	void X1ButtonDown();
	void X1ButtonUp();
	void X2ButtonDown();
	void X2ButtonUp();

	bool AddElement(const std::shared_ptr<UIElement> newElement);
	size_t GetElementIndex(std::shared_ptr<UIElement> element);
	std::shared_ptr<UIElement> GetElement(size_t index);
	bool MoveElement(size_t from, size_t to);
	bool RemoveElement(size_t index);
	size_t GetElementCount();

	void HScroll(int delta);
	void VScroll(int delta);
	void MouseMove(int x, int y);
	void MouseOut();

	void GetMousePosition(int &x, int &y);

	void SetWindowSize(int x, int y);
	void GetWindowSize(int &x, int &y);

	void CharTyped(wchar_t character);

	

	void RenderWireframe(std::shared_ptr<UIElement> &element);
	void Render(float deltaTime);

	static void Initialize(std::wstring shaderPath);
	
	static void CleanUp();

	struct ui_vertex { float x, y; };
	struct ui_texture_vertex { float x, y, u, v; };
	struct ui_color_vertex { float x, y, r, g, b, a; };
	struct ui_color_texture_vertex { float x, y, r, g, b, a, u, v; };

	static std::shared_ptr<Graphics::VertexShader> DebugVertexShader;
	static std::shared_ptr<Graphics::VertexShader> RectVertexShader;
	static std::shared_ptr<Graphics::VertexShader> AdvancedRectVertexShader;
	static std::shared_ptr<Graphics::PixelShader> PixelShader;

	static std::shared_ptr<Graphics::ConstantBuffer> MatrixBuffer;
	static std::shared_ptr<Graphics::ConstantBuffer> UIElementBuffer;
	static std::shared_ptr<Graphics::ConstantBuffer> MaterialBuffer;

	void UpdateActiveList(std::shared_ptr<UIElement> element);
	void UpdatePassiveList(std::shared_ptr<UIElement> element);

	void RebuildNonPassiveList();
	void MakeElementPassive(std::shared_ptr<UIElement> element);
private:
	void _UnMouseElement(std::shared_ptr<UIElement> &element);

	bool HasMouseOver;
	int MouseX;
	int MouseY;

	int WindowX;
	int WindowY;

	std::vector<std::shared_ptr<UIElement>> Elements;
	std::vector<std::shared_ptr<UIElement>> ActiveElements;
	std::vector<std::shared_ptr<UIElement>> NonPassiveElements;
	size_t FocusElementIndex = SIZE_MAX;
	size_t MouseOverElementIndex = SIZE_MAX;

public:
	Math::matrix4f WorldToScreen;

	static std::shared_ptr<Graphics::VertexBuffer> _DebugVtxBuffer;
	static std::shared_ptr<Graphics::IndexBuffer> _DebugIdxBuffer;
	static std::shared_ptr<Graphics::InputLayout> _DebugIL;
	static std::shared_ptr<Graphics::InputLayout> _AdvancedRectIL;
	static std::shared_ptr<Graphics::BlendState> _OpaqueBlend;
	static std::shared_ptr<Graphics::BlendState> _TransparentBlend;

	static std::shared_ptr<Graphics::Sampler> _PointFilter;
	static std::shared_ptr<Graphics::Sampler> _LinearFilter;

	static std::shared_ptr<Graphics::DepthStencilState> _DepthState;
	static bool DebugMode;

};

}}
