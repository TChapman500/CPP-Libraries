#include "ImageLoader.h"
#include <fstream>
#include "../zlib/zlib.h"

using TChapman500::Graphics::texture_data;
using TChapman500::Graphics::mip_data;
using TChapman500::Graphics::texture_format;
using TChapman500::Graphics::texture_usage;

namespace TChapman500 {
namespace Graphics {

ImageLoader::image ImageLoader::Load(std::wstring path)
{
	// Empty Result to be returned on error.
	image result;
	memset(&result, 0, sizeof(image));
	if (path.empty()) return result;

	size_t extIndex = path.find_last_of('.');
	if (extIndex == std::wstring::npos) return result;

	std::wstring extension = path.substr(extIndex + 1);
	for (size_t i = 0; i < extension.length(); i++)
		extension[i] = towlower(extension[i]);

	// Load TGA file.
	if (extension == L"tga") return LoadTGA(path);
	// Load BMP file.
	//if (extension == L"bmp") return LoadBMP(path);
	// Load PNG file.
	if (extension == L"png") return LoadPNG(path);
	// Load DDS file.
	//if (extension == L"dds") return LoadDDS(path);

	// No valid extension.
	return result;
}

ImageLoader::image ImageLoader::LoadTGA(std::wstring path)
{
	image result;
	memset(&result, 0, sizeof(image));
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) return result;
	result = LoadTGA(file);
	file.close();
	return result;
}

ImageLoader::image ImageLoader::LoadTGA(std::iostream &file)
{
	image result;
	memset(&result, 0, sizeof(image));

	bool colormap;
	char colortype;

	file.read((char *)&colormap, 1);	// Discard first byte.
	file.read((char *)&colormap, 1);
	file.read((char *)&colortype, 1);

	if (colormap || colortype != 2) return result;

	unsigned short width;
	unsigned short height;

	// Discard origin info
	file.read((char *)&width, 2);
	file.read((char *)&width, 2);
	file.read((char *)&width, 1);
	// Discard color map info
	file.read((char *)&width, 2);
	file.read((char *)&width, 2);

	// Keep width and height of image
	file.read((char *)&width, 2);
	file.read((char *)&height, 2);

	unsigned char bitsPerPixel;
	file.read((char *)&bitsPerPixel, 1);
	if (bitsPerPixel != 24 && bitsPerPixel != 32) return result;
	unsigned bytesPerPixel = bitsPerPixel >> 3;

	// Get the final flags before writing to the structure.
	unsigned char flags;
	file.read((char *)&flags, 1);
	bool rightToLeft = (flags >> 4) & 1;
	bool topToBottom = (flags >> 5) & 1;

	result.Width = width;
	result.Height = height;
	result.Format = texture_format::BGRA_8888;
	result.Textures = new texture_data[1];
	result.Textures[0].initialize(1, 1);
	result.Textures[0].ImageCount = 1;
	result.Textures[0].ArrayCount = 1;
	result.Textures[0].MipCount = 1;
	result.Textures[0].CubeMap = false;
	result.Textures[0].Transparent = false;
	result.Textures[0].Width = width;
	result.Textures[0].Height = height;
	result.Textures[0].Format = texture_format::BGRA_8888;
	result.Textures[0].Mips[0].Width = width;
	result.Textures[0].Mips[0].Height = height;
	result.Textures[0].Mips[0].BytesPerRow = 4 * width;
	result.Textures[0].Mips[0].DataSize = 4 * width * height;
	if (!result.Textures[0].generate_data())
	{
		delete[] result.Textures;
		result.Textures = nullptr;
		return result;
	}

	unsigned xStart;
	int xInc;
	unsigned xStop;
	if (rightToLeft)
	{
		xStart = result.Width - 1;
		xInc = -1;
		xStop = -1;
	}
	else
	{
		xStart = 0;
		xInc = 1;
		xStop = result.Width;
	}

	unsigned yStart;
	int yInc;
	unsigned yStop;
	if (topToBottom)
	{
		yStart = 0;
		yInc = 1;
		yStop = result.Height;
	}
	else
	{
		yStart = result.Height - 1;
		yInc = -1;
		yStop = -1;
	}

	char *mipData = (char *)result.Textures[0].get_data(0, 0);
	if (!mipData) return result;

	for (unsigned y = yStart; y != yStop; y += yInc)
	{
		for (unsigned x = xStart; x != xStop; x += xInc)
		{
			unsigned pos = 4 * (result.Width * y + x);
			if (pos + 3 >= result.Textures[0].Mips[0].DataSize)
			{
				int brk = pos;
				continue;
			}

			file.read(&mipData[pos], bytesPerPixel);
			if (bytesPerPixel == 3) mipData[pos + 3] = (char)255;
			else if (mipData[pos + 3] != 255) result.Textures[0].Transparent = true;
		}
	}

	return result;
}

ImageLoader::image ImageLoader::LoadPNG(std::wstring path)
{
	image result;
	memset(&result, 0, sizeof(image));
	std::fstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open()) return result;
	result = LoadPNG(file);
	file.close();
	return result;
}

ImageLoader::image ImageLoader::LoadPNG(std::iostream &file)
{
	image result;
	memset(&result, 0, sizeof(image));
	unsigned long long header;
	file.read((char *)&header, 8);
	if (header != 0x0A1A0A0D474E5089) return result;

	struct chunk
	{
		unsigned Length;
		unsigned Type;
		unsigned CRC;


		inline static unsigned ConvertInt(unsigned input) { return ((input >> 24) & 0xFF) | ((input >> 8) & 0xFF00) | ((input << 8) & 0xFF0000) | ((input << 24) & 0xFF000000); }
	};
	chunk chunkHeader;
	memset(&chunkHeader, 0, sizeof(chunk));

	struct chunk_IHDR
	{
		unsigned Width;
		unsigned Height;
		unsigned char BitDepth;
		unsigned char ColorType;
		unsigned short Zero;
		bool Interlace;
	};
	chunk_IHDR ihdr;
	ZeroMemory(&ihdr, sizeof(chunk_IHDR));

	struct chunk_IDAT
	{
		unsigned char *Buffer;
		size_t ChunkSize;
	};
	chunk_IDAT idat;
	idat.Buffer = nullptr;

	size_t bufferSize = 0;
	size_t bufferRead = 0;
	unsigned char *readBuffer = nullptr;
	unsigned char *readPos = nullptr;

	z_stream zs;
	memset(&zs, 0, sizeof(z_stream));
	if (inflateInit(&zs) != Z_OK)
	{
		DestroyImage(result);
		delete[] idat.Buffer;
		return result;
	}

	std::vector<chunk_IDAT> chunks;

	while (true)
	{
		file.read((char *)&chunkHeader.Length, 4);
		chunkHeader.Length = chunk::ConvertInt(chunkHeader.Length);

		file.read((char *)&chunkHeader.Type, 4);
		if (chunkHeader.Type == 0x444E4549)		// IEND
		{
			file.seekg(file.tellg() + (std::streampos)4);
			break;
		}
		if (chunkHeader.Type == 0x52444849)		// IHDR
		{
			file.read((char *)&ihdr.Width, 4);
			ihdr.Width = chunk::ConvertInt(ihdr.Width);

			file.read((char *)&ihdr.Height, 4);
			ihdr.Height = chunk::ConvertInt(ihdr.Height);

			file.read((char *)&ihdr.BitDepth, 1);
			if (ihdr.BitDepth != 8) return result;
			file.read((char *)&ihdr.ColorType, 1);
			if (ihdr.ColorType != 2 && ihdr.ColorType != 6) return result;
			file.read((char *)&ihdr.Zero, 2);
			if (ihdr.Zero != 0) return result;

			// We're not going to support this.
			file.read((char *)&ihdr.Interlace, 1);
			if (ihdr.Interlace) return result;

			// Skip CRC
			file.seekg(file.tellg() + (std::streampos)4);

			result.Width = ihdr.Width;
			result.Height = ihdr.Height;
			result.Format = texture_format::RGBA_8888;
			result.Textures = new texture_data[1];
			result.Textures[0].ArrayCount = 1;
			result.Textures[0].CubeMap = false;
			result.Textures[0].Format = texture_format::RGBA_8888;
			result.Textures[0].Width = result.Width;
			result.Textures[0].Height = result.Height;
			result.Textures[0].ImageCount = 1;
			result.Textures[0].MipCount = 1;
			result.Textures[0].ArrayCount = 1;
			result.Textures[0].Transparent = ihdr.ColorType == 6;
			result.Textures[0].Mips = new mip_data[1];
			result.Textures[0].Mips[0].Width = result.Width;
			result.Textures[0].Mips[0].Height = result.Height;
			result.Textures[0].Mips[0].BytesPerRow = result.Width * 4;
			result.Textures[0].Mips[0].DataSize = result.Textures[0].Mips[0].BytesPerRow * result.Height;
			result.Textures[0].Data = new unsigned char *[1];
			result.Textures[0].Data[0] = new unsigned char[result.Textures[0].Mips[0].DataSize];

			if (ihdr.ColorType == 2) bufferSize = ((size_t)result.Width * 3 + 1) * (size_t)result.Height;
			else bufferSize = ((size_t)result.Width * 4 + 1) * (size_t)result.Height;
			readBuffer = new unsigned char[bufferSize];
			readPos = readBuffer;
		}
		else if (chunkHeader.Type == 0x54414449)	// IDAT
		{
			idat.Buffer = new unsigned char[chunkHeader.Length];
			idat.ChunkSize = chunkHeader.Length;
			// Read the chunk data
			file.read((char*)idat.Buffer, chunkHeader.Length);
			file.seekg(file.tellg() + (std::streampos)4);
			chunks.push_back(idat);
		}
		else file.seekg(file.tellg() + (std::streampos)chunkHeader.Length + (std::streampos)4);
	}

	size_t totalSize = 0;
	for (const chunk_IDAT &chunk : chunks) totalSize += chunk.ChunkSize;
	unsigned char *fullBuffer = new unsigned char[totalSize];
	unsigned char *currPos = fullBuffer;
	for (const chunk_IDAT &chunk : chunks)
	{
		memcpy_s(currPos, chunk.ChunkSize, chunk.Buffer, chunk.ChunkSize);
		currPos += chunk.ChunkSize;
		delete[] chunk.Buffer;
	}

	//*
	zs.next_in = (Bytef *)fullBuffer;
	zs.avail_in = (uInt)totalSize;
	zs.next_out = (Bytef *)readPos;
	zs.avail_out = (uInt)bufferSize;

	int ret = inflate(&zs, Z_NO_FLUSH);
	if (ret != Z_STREAM_END && ret != Z_OK)
	{
		inflateEnd(&zs);
		DestroyImage(result);
		delete[] fullBuffer;
		return result;
	}

	//*/



	inflateEnd(&zs);

	// Clean-up!
	if (fullBuffer) delete[] fullBuffer;

	auto paethPredictor = [](unsigned char a, unsigned char b, unsigned char c) -> unsigned char {

		short p = (short)a + (short)b - (short)c;
		short pa = abs(p - (short)a);
		short pb = abs(p - (short)b);
		short pc = abs(p - (short)c);
		if (pa <= pb && pa <= pc) return a;
		if (pb <= pc) return b;
		return c;
	};
	auto runScanline = [paethPredictor](unsigned char *scanline, size_t scanlineLength, size_t bytesPerPixel, size_t scanlineIndex) -> void {
		unsigned char filter = *scanline;
		switch (filter)
		{
		case 0:		// No Filter
			break;
		case 1:		// Sub
			for (size_t x = 1; x < scanlineLength; x++)
			{
				if (x >= bytesPerPixel + 1) scanline[x] += scanline[x - bytesPerPixel];
			}
			break;
		case 2:		// Up
			for (size_t x = 1; x < scanlineLength; x++)
			{
				if (scanlineIndex > 0)
					scanline[x] += scanlineIndex > 0 ? scanline[x - scanlineLength] : 0;
			}
			break;
		case 3:		// Average
			for (size_t x = 1; x < scanlineLength; x++)
			{
				unsigned char a = 0;
				unsigned char b = scanlineIndex > 0 ? scanline[x - scanlineLength] : 0;

				if (x >= bytesPerPixel + 1) a = scanline[x - bytesPerPixel];

				scanline[x] += (a + b) >> 1;
			}
			break;
		case 4:		// Paeth
			for (size_t x = 1; x < scanlineLength; x++)
			{
				unsigned char a = 0;
				unsigned char b = 0;
				unsigned char c = 0;

				if (x >= bytesPerPixel + 1)
				{
					a = scanline[x - bytesPerPixel];
					if (scanlineIndex > 0)
					{
						b = scanline[x - scanlineLength];
						c = scanline[x - (scanlineLength + bytesPerPixel)];
					}
				}
				else if (scanlineIndex > 0) b = scanline[x - scanlineLength + bytesPerPixel];
				scanline[x] += paethPredictor(a, b, c);
			}
			break;
		}
	};

	size_t bytesPerPixel = ihdr.ColorType == 6 ? 4 : 3;
	size_t scanlineLength = result.Width * bytesPerPixel + 1;
	unsigned char *dest = result.Textures[0].Data[0];

	for (unsigned y = 0; y < result.Height; y++)
	{
		size_t scanlineIndex = y;
		unsigned char *scanline = readBuffer + (y * scanlineLength);
		runScanline(scanline, scanlineLength, bytesPerPixel, scanlineIndex);
		if (ihdr.ColorType == 6)
		{
			for (unsigned x = 0; x < result.Width; x++)
			{
				unsigned iByte = (y * (result.Width * 4 + 1) + x * 4) + 1;
				unsigned oByte = y * result.Width * 4 + x * 4;
				dest[oByte + 0] = readBuffer[iByte + 0];
				dest[oByte + 1] = readBuffer[iByte + 1];
				dest[oByte + 2] = readBuffer[iByte + 2];
				dest[oByte + 3] = readBuffer[iByte + 3];
			}
		}
		else
		{
			for (unsigned x = 0; x < result.Width; x++)
			{
				unsigned iByte = (y * (result.Width * 3 + 1) + x * 3) + 1;
				unsigned oByte = y * result.Width * 4 + x * 4;
				dest[oByte + 0] = readBuffer[iByte + 0];
				dest[oByte + 1] = readBuffer[iByte + 1];
				dest[oByte + 2] = readBuffer[iByte + 2];
				dest[oByte + 3] = 255;
			}
		}
	}
	delete[] readBuffer;

	return result;
}

void ImageLoader::DestroyImage(ImageLoader::image &image)
{
	delete[] image.Textures;
	memset(&image, 0, sizeof(ImageLoader::image));
}

std::shared_ptr<ShaderResourceView> CreateShaderResource(std::wstring path)
{
	std::shared_ptr<Texture> texture = CreateTexture(path);
	return std::make_shared<ShaderResourceView>(texture);
}

std::shared_ptr<Texture> CreateTexture(std::wstring path)
{
	ImageLoader::image image = ImageLoader::Load(path);
	if (image.Width == 0 || image.Height == 0) return nullptr;
	std::shared_ptr<Texture> result = std::make_shared<Texture>(image.Textures[0]);
	ImageLoader::DestroyImage(image);
	return result;
}

}}
