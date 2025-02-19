#include "UIImage.h"
#include "UISystem.h"
#include "../Graphics/Graphics_Core.h"

namespace TChapman500 {
namespace GUI {

UIImage::UIImage(UISystem *uiSystem) : UIElement(uiSystem)
{
	_DrawCount = 6;

	TopSplit = 0;
	BottomSplit = 0;
	LeftSplit = 0;
	RightSplit = 0;

	float initVertices[] = {
		0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f
	};
	unsigned initIndices[] = {0, 1, 3, 0, 3, 2 };
	Color = Math::vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	_Vertices = std::make_shared<Graphics::VertexBuffer>(8, 16, nullptr, false);
	_Indices = std::make_shared<Graphics::IndexBuffer>(54, nullptr, false);

	_Vertices->WriteBuffer(initVertices, sizeof(float) * 8 * 4);
	_Indices->WriteBuffer(initIndices, sizeof(unsigned) * 6);

	PreviousHasBorder = false;

}
UIImage::~UIImage()
{
}
void UIImage::Render(float deltaTime)
{
	matrix_buffer matrixData = { LocalToWorld, Window->WorldToScreen };
	Window->MatrixBuffer->WriteBuffer(&matrixData);
	element_buffer elementData = { Pivot, Size };
	Window->UIElementBuffer->WriteBuffer(&elementData);

	material_buffer materialData = { Color, _Texture ? 1U : 0 };
	Window->MaterialBuffer->WriteBuffer(&materialData);

	UISystem::AdvancedRectVertexShader->Set();

	UISystem::_TransparentBlend->Set();
	UISystem::_AdvancedRectIL->Set();

	if (_Texture)
	{
		_Texture->Set(0);
		_Sampler->Set(0);
	}

	_Vertices->Set(0);
	_Indices->Set(0);

	Graphics::SetTopology(Graphics::topology::TriList);
	Graphics::DrawIndexed(_DrawCount);

	UIElement::Render(deltaTime);
}

bool UIImage::SetSize(float x, float y)
{
	if (!UIElement::SetSize(x, y)) return false;
	_GenerateMesh();
	return true;
}

bool UIImage::SetAnchors(float minX, float minY, float maxX, float maxY)
{
	if (!UIElement::SetAnchors(minX, minY, maxX, maxY)) return false;
	_GenerateMesh();
	return true;
}

bool UIImage::SetOffsets(float minX, float minY, float maxX, float maxY)
{
	if (!UIElement::SetOffsets(minX, minY, maxX, maxY)) return false;
	_GenerateMesh();
	return true;
}

void UIImage::SetTexture(std::shared_ptr<Graphics::ShaderResourceView> &texture)
{
	bool changed = _Texture != texture;
	_Texture = texture;
	if (!_Sampler) _Sampler = UISystem::_LinearFilter;
	if (changed) _GenerateMesh();
}

void UIImage::SetTexture(std::shared_ptr<Graphics::ShaderResourceView> &texture, bool pointFiltering)
{
	bool changed = _Texture != texture;
	_Texture = texture;

	if (pointFiltering) _Sampler = UISystem::_PointFilter;
	else _Sampler = UISystem::_LinearFilter;

	if (changed) _GenerateMesh();
}

void UIImage::SetSplits(float top, float bottom, float left, float right)
{
	if (top < 0.0f) top = 0.0f;
	if (bottom < 0.0f) bottom = 0.0f;
	if (left < 0.0f) left = 0.0f;
	if (right < 0.0f) right = 0.0f;

	TopSplit = top;
	BottomSplit = bottom;
	LeftSplit = left;
	RightSplit = right;

	_GenerateMesh();
}

void UIImage::_GenerateMesh()
{
	struct vertex
	{
		float X, Y;
		float R, G, B, A;
		float U, V;
	};
	vertex vertices[16];

	unsigned flags = 0;
	if (TopSplit) flags |= 1;
	if (BottomSplit) flags |= 2;
	if (LeftSplit) flags |= 4;
	if (RightSplit) flags |= 8;

	if (!_Texture || !flags)
	{
		vertices[0] = { 0.0f,   0.0f,    1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f };
		vertices[1] = { Size.X, 0.0f,    1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f };
		vertices[2] = { 0.0f,   Size.Y,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f };
		vertices[3] = { Size.X, Size.Y,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f };
		unsigned indices[] = { 0, 1, 3, 0, 3, 2 };
		_Vertices->WriteBuffer(vertices, sizeof(vertex) * 4);
		_Indices->WriteBuffer(indices, sizeof(unsigned) * 6);
		_DrawCount = 6;
		return;
	}

	float leftU = LeftSplit / (float)_Texture->ITexture->Width;
	float rightU = 1.0f - RightSplit / (float)_Texture->ITexture->Width;
	float topV = TopSplit / (float)_Texture->ITexture->Height;
	float bottomV = 1.0f - BottomSplit / (float)_Texture->ITexture->Height;

	_DrawCount = 54;

	vertices[0]  = { 0.0f,                0.0f,                 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   0.0f };
	vertices[1]  = { LeftSplit,           0.0f,                 1.0f, 1.0f, 1.0f, 1.0f, leftU,  0.0f };
	vertices[2]  = { Size.X - RightSplit, 0.0f,                 1.0f, 1.0f, 1.0f, 1.0f, rightU, 0.0f };
	vertices[3]  = { Size.X,              0.0f,                 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   0.0f };

	vertices[4]  = { 0.0f,                TopSplit,             1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   topV };
	vertices[5]  = { LeftSplit,           TopSplit,             1.0f, 1.0f, 1.0f, 1.0f, leftU,  topV };
	vertices[6]  = { Size.X - RightSplit, TopSplit,             1.0f, 1.0f, 1.0f, 1.0f, rightU, topV };
	vertices[7]  = { Size.X,              TopSplit,             1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   topV };

	vertices[8]  = { 0.0f,                Size.Y - BottomSplit, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   bottomV };
	vertices[9]  = { LeftSplit,           Size.Y - BottomSplit, 1.0f, 1.0f, 1.0f, 1.0f, leftU,  bottomV };
	vertices[10] = { Size.X - RightSplit, Size.Y - BottomSplit, 1.0f, 1.0f, 1.0f, 1.0f, rightU, bottomV };
	vertices[11] = { Size.X,              Size.Y - BottomSplit, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   bottomV };

	vertices[12] = { 0.0f,                Size.Y,               1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   1.0f };
	vertices[13] = { LeftSplit,           Size.Y,               1.0f, 1.0f, 1.0f, 1.0f, leftU,  1.0f };
	vertices[14] = { Size.X - RightSplit, Size.Y,               1.0f, 1.0f, 1.0f, 1.0f, rightU, 1.0f };
	vertices[15] = { Size.X,              Size.Y,               1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   1.0f };

	unsigned indices[] = {
		 0,  1,  5,  0,  5,  4,
		 1,  2,  6,  1,  6,  5,
		 2,  3,  7,  2,  7,  6,
		 4,  5,  9,  4,  9,  8,
		 5,  6, 10,  5, 10,  9,
		 6,  7, 11,  6, 11, 10,
		 8,  9, 13,  8, 13, 12,
		 9, 10, 14,  9, 14, 13,
		10, 11, 15, 10, 15, 14
	};

	_Vertices->WriteBuffer(vertices, sizeof(vertex) * 16);
	_Indices->WriteBuffer(indices, sizeof(unsigned) * 54);
}

}}
