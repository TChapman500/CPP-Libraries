#pragma once
#include "../Graphics/Buffers.h"
#include "UIFont.h"
#include <memory>
#include <string>
#include <vectors.h>

namespace TChapman500 {
namespace GUI {

class PregeneratedText
{
public:
	PregeneratedText();
	~PregeneratedText();

	void SetAlignment(UIFont::align hAlignment, UIFont::align vAlignment);
	bool Generate(std::wstring &text);

	void Render();

	float Width;
	float Height;
	float LineHeight;
	UIFont::align HAlignment;
	UIFont::align VAlignment;
	std::shared_ptr<UIFont> _Font;

	Math::vector4f Color;

	std::wstring Text;

	size_t _CharacterCapacity;
	size_t _CharacterCount;

	// First line to render
	size_t StartLine;
	// Last line to render
	size_t EndLine;
	// Line bounds in characters, multiply by 6 to get rect indices.
	// Start Index = first * 6
	// Index Count = ((last + 1) * 6) - (first * 6)
	std::vector<std::pair<size_t, size_t>> LineBounds;

	std::shared_ptr<Graphics::VertexBuffer> _VtxBuffer;
	std::shared_ptr<Graphics::IndexBuffer> _IdxBuffer;
};

}}