#pragma once
#include <d3d11.h>
#include <memory>

namespace TChapman500 {
namespace Graphics {

extern ID3D11Device *Device;
extern ID3D11DeviceContext *Context;

extern D3D11_BUFFER_DESC BufferDesc;
extern D3D11_MAPPED_SUBRESOURCE MappedBufferSubresource;

enum class texture_format { BGR_565, BGRA_4444, BGRA_5551, BGRA_8888, RGBA_8888, DXT1, DXT3, DXT5, D32, D24_S8, R_FLOAT, RG_FLOAT, RGB_FLOAT, RGBA_FLOAT };

struct mip_data
{
	unsigned Width;
	unsigned Height;
	unsigned BytesPerRow;
	unsigned DataSize;
};

struct texture_data
{
	unsigned Width;
	unsigned Height;
	unsigned MipCount;	// Per Array Element
	unsigned ArrayCount;	// Number of Elements in the Array
	unsigned ImageCount;	// Mips * Elements
	texture_format Format;
	bool CubeMap;
	bool Transparent;
	mip_data *Mips;
	unsigned char **Data;

	inline texture_data()
	{
		Mips = nullptr;
		Data = nullptr;
	}

	inline void initialize(unsigned mipCount, unsigned arrayCount)
	{
		ArrayCount = arrayCount;
		MipCount = mipCount;
		Data = nullptr;

		try
		{
			Mips = new mip_data[mipCount];
			memset(Mips, 0, sizeof(mip_data) * mipCount);
		}
		catch (std::bad_alloc) { Mips = nullptr; }
	}

	inline bool generate_data()
	{
		// Cube maps must have a multiple of 6 images.
		if (CubeMap && (ArrayCount % 6 != 0) || !Mips) return false;

		// Calculate Array Properties
		unsigned imageCount = ArrayCount * MipCount;

		unsigned char **tempData;
		try { tempData = new unsigned char *[imageCount]; }
		catch (std::bad_alloc) { return false; }

		for (unsigned i = 0; i < imageCount; i++) tempData[i] = nullptr;

		bool error = false;
		// Attempt to allocate memory for the image data.
		for (unsigned element = 0; element < ArrayCount; element++)
		{
			for (unsigned mip = 0; mip < MipCount; mip++)
			{
				unsigned image = element * MipCount + mip;
				unsigned size = Mips[mip].DataSize;
				if (image >= imageCount)
				{
					int brk = 0;
					continue;
				}

				tempData[image] = new(std::nothrow) unsigned char[size];
				if (!tempData[image])
				{
					error = true;
					break;
				}
			}
			if (error) break;
		}
		
		// Problem allocating memory.
		if (error)
		{
			for (unsigned i = 0; i < imageCount; i++)
			{
				if (tempData[i]) delete[] tempData[i];
				else break;
			}
			delete[] tempData;
			return false;
		}

		// Everything worked out.
		ImageCount = imageCount;
		delete_data();
		Data = tempData;
		return true;
	}

	inline void delete_data()
	{
		if (Data)
		{
			for (unsigned i = 0; i < ImageCount; i++)
				if (Data[i]) delete[] Data[i];

			delete[] Data;
			Data = nullptr;
		}
	}

	inline bool set_data(unsigned mipIndex, unsigned arrayIndex, unsigned dataSizeBytes, unsigned char *data)
	{
		if (!Data) return false;
		if (mipIndex >= MipCount || arrayIndex >= ArrayCount) return false;
		if (dataSizeBytes > Mips[mipIndex].DataSize) return false;

		unsigned imageIndex = MipCount * arrayIndex + mipIndex;
		if (!Data[imageIndex]) return false;
		memcpy_s(Data[imageIndex], Mips[mipIndex].DataSize, data, dataSizeBytes);
	}

	inline unsigned char *get_data(unsigned mipIndex, unsigned arrayIndex)
	{
		if (!Data) return nullptr;
		if (mipIndex >= MipCount || arrayIndex >= ArrayCount) return nullptr;

		unsigned imageIndex = MipCount * arrayIndex + mipIndex;
		return Data[imageIndex];
	}

	inline ~texture_data()
	{
		if (Mips)
		{
			delete[] Mips;
			Mips = nullptr;
		}
		delete_data();
	}
};

enum class texture_usage
{
	Undefined,
	Resource,
	DepthStencil,
	ResourceAndTarget,	// Texture can be used as both a render target and shader resource.
	TargetOnly,		// Texture can only be used as a render target.
	Output			// Texture is used only as output from the GPU.
};

struct il_create_param
{
	char *Semantic;
	unsigned SemanticIndex;
	unsigned FloatCount;
	unsigned InputSlot;
	unsigned InstanceStepRate;
	bool IsInstanced;
};

enum class sampler_filter
{
	Point,
	Bilinear,
	Trilinear,
	Anisotropic2x,
	Anisotropic4x,
	Anisotropic8x,
	Anisotropic16x
};

enum class sampler_mode
{
	Clamp,
	Tile,
	Border
};

struct viewport
{
	float Top;
	float Left;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
};

enum class stencil_op
{
	Disable,
	Test,
	Write
};

enum class topology { PointList, LineList, LineStrip, TriList, TriStrip };

}}
