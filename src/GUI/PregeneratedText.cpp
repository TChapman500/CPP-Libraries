#include "PregeneratedText.h"
#include "UISystem.h"
#include "../Graphics/Graphics_Core.h"

namespace TChapman500 {
namespace GUI {

PregeneratedText::PregeneratedText()
{
	// Index Capacity = 6 * Character Capacity
	// Vertex Capacity = 4 * Character Capacity
	_CharacterCapacity = 0;
	_CharacterCount = 0;

	Width = 0;
	Height = 0;
	LineHeight = 0;
	HAlignment = UIFont::align::Center;
	VAlignment = UIFont::align::Center;

	Color = Math::vector4f(1.0f, 1.0f, 1.0f, 1.0f);
}

PregeneratedText::~PregeneratedText()
{
}

void PregeneratedText::SetAlignment(UIFont::align hAlignment, UIFont::align vAlignment)
{
	HAlignment = hAlignment;
	VAlignment = vAlignment;

	if (!Text.empty()) Generate(Text);
}

bool PregeneratedText::Generate(std::wstring &text)
{
	if (!_Font) return false;

	// Generate Character Data
	Text = text;
	if (Text.empty()) return false;

	UIFont::gen_data genData;
	genData.Width = Width;
	genData.Height = Height;
	genData.LineHeight = LineHeight;
	genData.HorizontalAlign = HAlignment;
	genData.VerticalAlign = VAlignment;
	_Font->GenerateVertices((wchar_t *)text.c_str(), &genData);
	_CharacterCount = genData.VertexCount >> 2;
	if (_CharacterCount > _CharacterCapacity)
	{
		_VtxBuffer = std::make_shared<Graphics::VertexBuffer>(8, genData.VertexCount, (float *)genData.VertexList, false);
		_IdxBuffer = std::make_shared<Graphics::IndexBuffer>(genData.IndexCount, genData.IndexList, false);

		_CharacterCapacity = _CharacterCount;
	}
	else if (_VtxBuffer && _IdxBuffer)
	{
		_VtxBuffer->WriteBuffer(genData.VertexList, genData.VertexCount * sizeof(UIFont::char_vertex));
		_IdxBuffer->WriteBuffer(genData.IndexList, (size_t)genData.IndexCount * 4);
	}

	LineBounds.clear();
	if (_CharacterCount != 0)
	{
		LineBounds.push_back(std::pair<size_t, size_t>(0, 0));
		StartLine = 0;

		float startY = genData.VertexList[0].Position[1];
		for (size_t i = 0, l = 0; i < _CharacterCount; i++)
		{
			if (genData.VertexList[i * 4].Position[1] == startY) LineBounds[l].second++;
			else
			{
				LineBounds.push_back(std::pair<size_t, size_t>(i, i));
				startY = genData.VertexList[i * 4].Position[1];
				l++;
			}
		}

		EndLine = LineBounds.size() - 1;
	}
	else
	{
		StartLine = 0;
		EndLine = 0;
	}

	delete[] genData.VertexList;
	delete[] genData.IndexList;
	return true;
}

void PregeneratedText::Render()
{
	if (Text.empty()) return;
	
	UISystem::_TransparentBlend->Set();
	UISystem::_AdvancedRectIL->Set();
	_Font->Texture->Set(0);
	_Font->Sampler->Set(0);
	_VtxBuffer->Set(0);
	_IdxBuffer->Set(0);
	Graphics::SetTopology(Graphics::topology::TriList);
	Graphics::DrawIndexed((unsigned)_CharacterCount * 6);

}



}}