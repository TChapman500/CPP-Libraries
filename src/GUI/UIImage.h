#pragma once
#include "UIElement.h"
#include "../Graphics/States.h"
#include "../Graphics/Textures.h"
#include "../Graphics/Buffers.h"
#include <memory>

namespace TChapman500 {
namespace GUI {

class UIImage : public UIElement
{
public:
	UIImage(UISystem *uiSystem);
	~UIImage();

	virtual void Render(float deltaTime) override;

	virtual bool SetSize(float x, float y) override;
	virtual bool SetAnchors(float minX, float minY, float maxX, float maxY) override;
	virtual bool SetOffsets(float minX, float minY, float maxX, float maxY) override;

	void SetTexture(std::shared_ptr<Graphics::ShaderResourceView> &texture);
	void SetTexture(std::shared_ptr<Graphics::ShaderResourceView> &texture, bool pointFiltering);

	void SetSplits(float top, float bottom, float left, float right);

	Math::vector4f Color;


private:
	void _GenerateMesh();

	float TopSplit;
	float BottomSplit;
	float LeftSplit;
	float RightSplit;

	unsigned _DrawCount;
	bool PreviousHasBorder;

	std::shared_ptr<Graphics::ShaderResourceView> _Texture;
	std::shared_ptr<Graphics::Sampler> _Sampler;
	std::shared_ptr<Graphics::VertexBuffer> _Vertices;
	std::shared_ptr<Graphics::IndexBuffer> _Indices;
};

}}
