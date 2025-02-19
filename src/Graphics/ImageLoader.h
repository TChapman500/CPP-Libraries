#pragma once
#include <string>
#include "Textures.h"

namespace TChapman500 {
namespace Graphics {

class ImageLoader
{
public:
	struct image
	{
		unsigned Width;
		unsigned Height;
		TChapman500::Graphics::texture_format Format;
		TChapman500::Graphics::texture_data *Textures;
	};

	static image Load(std::wstring path);

	static image LoadTGA(std::wstring path);
	static image LoadTGA(std::iostream &file);

	static image LoadPNG(std::wstring path);
	static image LoadPNG(std::iostream &file);


	static void DestroyImage(image &image);
};

std::shared_ptr<ShaderResourceView> CreateShaderResource(std::wstring path);
std::shared_ptr<Texture> CreateTexture(std::wstring path);

}}

