#pragma once
#include "Graphics.h"
#include <dxgi.h>
#include <vector>
#include <memory>
#include <string>

namespace TChapman500 {
namespace Graphics {

inline bool isPowerOfTwo(unsigned value) { return (value & (value - 1)) == 0; }
inline unsigned getBytesPerRow(texture_format format, mip_data *mip)
{
	// Verify bytes per row for DXT-compressed textures.
	switch (format)
	{
	case texture_format::BGRA_5551:
	case texture_format::BGRA_4444:
	case texture_format::BGR_565:
	{
		unsigned bpr = mip->Width * 2;
		if (bpr % 4 != 0) bpr += 2;
		return bpr;
	}
	case texture_format::BGRA_8888:
	case texture_format::RGBA_8888:
	case texture_format::D24_S8:
	case texture_format::D32:
	case texture_format::R_FLOAT:
		return mip->Width * 4;
	case texture_format::RG_FLOAT:
		return mip->Width * 8;
	case texture_format::RGB_FLOAT:
		return mip->Width * 12;
	case texture_format::RGBA_FLOAT:
		return mip->Width * 16;
	case texture_format::DXT1:
	{
		unsigned bpr = 8 * (mip->Width / 4);
		if (mip->Width % 4 != 0) bpr += 8;
		return bpr;
	}
	case texture_format::DXT3:
	case texture_format::DXT5:
	{
		unsigned bpr = 16 * (mip->Width / 4);
		if (mip->Width % 4 != 0) bpr += 16;
		return bpr;
	}
	default:
		return 0;
		break;
	}
}
inline unsigned getDataSize(texture_format format, mip_data *mip)
{
	// The default data size.
	unsigned result = mip->BytesPerRow * mip->Height;

	// Calculate total data size for DXT-compressed textures.
	switch (format)
	{
	case texture_format::DXT1:
	{
		unsigned bpr = 8 * (mip->Width / 4);
		if (mip->Width % 4 != 0) bpr += 8;
		return bpr;
	}
	case texture_format::DXT3:
	case texture_format::DXT5:
	{
		unsigned bpr = 16 * (mip->Width / 4);
		if (mip->Width % 4 != 0) bpr += 16;
		return bpr;
	}
	}

	// The final result.
	return result;
}
inline bool verifyMip(unsigned width, unsigned height, texture_format format, unsigned mipIndex, mip_data *mip)
{
	// Expected with of the mip in pixels.
	unsigned expectedWidth = mip->Width >> mipIndex;
	if (expectedWidth == 0) expectedWidth = 1;

	// Expected height of the mip in pixels.
	unsigned expectedHeight = mip->Height >> mipIndex;
	if (expectedHeight == 0) expectedHeight = 1;

	// Verify that the mip meets the expected parameters.
	if (mip->Width != expectedWidth) return false;
	if (mip->Height != expectedHeight) return false;
	if (mip->BytesPerRow % 4 != 0) return false;

	// Verify that mip size is accurate.
	if (mip->BytesPerRow != getBytesPerRow(format, mip)) return false;
	if (mip->DataSize != getDataSize(format, mip)) return false;

	// Everything checks out
	return true;
}
inline DXGI_FORMAT formatConverter(texture_format format)
{
	const DXGI_FORMAT outputs[] = {
		DXGI_FORMAT::DXGI_FORMAT_B5G6R5_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_B4G4R4A4_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_B5G5R5A1_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_BC2_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM,
		DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT
	};
	return outputs[(size_t)format];
}
inline texture_format formatConverter(DXGI_FORMAT format)
{
	const texture_format outputs[] = {
		texture_format::BGR_565,
		texture_format::BGRA_4444,
		texture_format::BGRA_5551,
		texture_format::BGRA_8888,
		texture_format::RGBA_8888,
		texture_format::DXT1,
		texture_format::DXT3,
		texture_format::DXT5,
		texture_format::D32,
		texture_format::D24_S8,
		texture_format::R_FLOAT,
		texture_format::RG_FLOAT,
		texture_format::RGB_FLOAT,
		texture_format::RGBA_FLOAT
	};
	return outputs[(size_t)format];
}

class Texture
{
public:

	// Use this if you want to load a texture
	// to be used only as a shader resource.
	inline Texture(texture_data &data)
	{
		// Run verification checks.
		if (!data.Mips) return;
		if (!data.MipCount) return;
		if (!data.Width || !data.Height) return;
		if (!data.Mips[0].BytesPerRow || !data.Mips[0].DataSize) return;
		if (data.CubeMap && (data.ArrayCount % 6 != 0)) return;

		// Initialize Basic Texture Data
		Width = data.Width;
		Height = data.Height;
		MipCount = data.MipCount;
		Format = formatConverter(data.Format);
		CubeMap = data.CubeMap;
		Transparent = data.Transparent;

		// Verify all mip levels.
		for (unsigned i = 0; i < MipCount; i++)
			if (!verifyMip(Width, Height, data.Format, i, &data.Mips[i])) return;

		// Store Mip Data
		MipData = new mip_data[MipCount];
		for (unsigned i = 0; i < MipCount; i++)
		{
			MipData[i].Width = data.Mips[i].Width;
			MipData[i].Height = data.Mips[i].Height;
			MipData[i].BytesPerRow = data.Mips[i].BytesPerRow;
			MipData[i].DataSize = data.Mips[i].DataSize;
		}

		// Create the Texture
		{
			// Texture description to create the texture.
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

			// Populate texture description.
			texDesc.Width = Width;
			texDesc.Height = Height;
			texDesc.MipLevels = MipCount;
			texDesc.Format = Format;
			texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			texDesc.ArraySize = data.ArrayCount;

			// For cube maps
			if (CubeMap) texDesc.MiscFlags |= D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;

			// Generate subresource data and create the texture.
			unsigned subCount = texDesc.ArraySize * texDesc.MipLevels;
			D3D11_SUBRESOURCE_DATA *subData = new D3D11_SUBRESOURCE_DATA[subCount];
			for (unsigned i = 0, mip = 0, side = 0; i < subCount; i++)
			{
				// Get side and mip index
				side = i % MipCount;
				mip = i - (side * MipCount);

				// Populate data field.
				subData->SysMemPitch = data.Mips[mip].BytesPerRow;
				subData->SysMemSlicePitch = 0;
				subData->pSysMem = data.Data[i];
			}
			HRESULT result = Device->CreateTexture2D(&texDesc, subData, &ITexture);
			delete[] subData;

			// Can't do anything more here.
			if (FAILED(result)) return;
		}
	}

	// Use this if you want to create a render target or output texture.
	inline Texture(unsigned width, unsigned height, texture_format format, bool genMips, bool cubeMap, texture_usage usage)
	{
		Width = width;
		Height = height;
		Format = formatConverter(format);
		CubeMap = cubeMap;
		Transparent = true;
		ArrayCount = cubeMap ? 6 : 1;

		if (genMips)
		{
			// Get number of mips.
			MipCount = 0;
			unsigned maxSize = width >= height ? width : height;
			while (maxSize)
			{
				MipCount++;
				maxSize >>= 1;
			}
		}
		else MipCount = 1;

		// Populate mip data
		MipData = new mip_data[MipCount];
		for (unsigned i = 0; i < MipCount; i++)
		{
			MipData[i].Width = Width >> i;
			if (MipData[i].Width == 0) MipData[i].Width = 1;

			MipData[i].Height = Height >> i;
			if (MipData[i].Height == 0) MipData[i].Height = 1;

			MipData[i].BytesPerRow = getBytesPerRow(format, &MipData[i]);
			MipData[i].DataSize = getDataSize(format, &MipData[i]);
		}

		// Create the texture
		{
			// Texture description to create the texture.
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

			// Populate texture description.
			texDesc.Width = Width;
			texDesc.Height = Height;
			texDesc.MipLevels = MipCount;
			texDesc.Format = Format;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;

			// How will this generated texture be used.
			switch (usage)
			{
			case texture_usage::DepthStencil:
				texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
				break;
			case texture_usage::ResourceAndTarget:
				texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
				break;
			case texture_usage::TargetOnly:
				texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
				break;
			case texture_usage::Output:
				texDesc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
				texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
				break;
			}

			// Whether we are allowed to generate mips with this resource. 
			if (usage != texture_usage::DepthStencil && genMips) texDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_GENERATE_MIPS;

			// For Cube Maps
			if (usage != texture_usage::DepthStencil && cubeMap)
			{
				texDesc.MiscFlags |= D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
				texDesc.ArraySize = 6;
			}
			else texDesc.ArraySize = 1;

			// Create the texture
			HRESULT result = Device->CreateTexture2D(&texDesc, nullptr, &ITexture);
			if (FAILED(result)) return;
		}
	}

	// Create a texture created by a different interface.
	inline Texture(void *texture)
	{
		ITexture = (ID3D11Texture2D*)texture;
		ITexture->AddRef();

		D3D11_TEXTURE2D_DESC texDesc;
		ITexture->GetDesc(&texDesc);

		// Get usage data.
		bool renderTarget = texDesc.BindFlags && D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		bool shaderResource = texDesc.BindFlags && D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		bool depthStencil = texDesc.BindFlags && D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		bool output = texDesc.Usage == D3D11_USAGE::D3D11_USAGE_STAGING;

		// Set usage flag.
		if (renderTarget && shaderResource) Usage = texture_usage::ResourceAndTarget;
		else if (renderTarget) Usage = texture_usage::TargetOnly;
		else if (shaderResource) Usage = texture_usage::Resource;
		else if (depthStencil) Usage = texture_usage::DepthStencil;
		else if (output) Usage = texture_usage::Output;
		else Usage = texture_usage::Undefined;

		// Texture is a cube map
		CubeMap = texDesc.MiscFlags && D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Get dimensions and format.
		Width = texDesc.Width;
		Height = texDesc.Height;
		Format = texDesc.Format;
		MipCount = texDesc.MipLevels;
		Transparent = true;

		// Populate Mip Level Data
		MipData = new mip_data[MipCount];
		for (unsigned i = 0; i < MipCount; i++)
		{
			MipData[i].Width = Width >> i;
			if (MipData[i].Width == 0) MipData[i].Width = 1;

			MipData[i].Height = Height >> i;
			if (MipData[i].Height == 0) MipData[i].Height = 1;

			MipData[i].BytesPerRow = getBytesPerRow(formatConverter(Format), &MipData[i]);
			MipData[i].DataSize = getDataSize(formatConverter(Format), &MipData[i]);
		}
	}

	inline virtual ~Texture()
	{
		ITexture->Release();
		delete[] MipData;
	}

	inline static bool CopyTexture(Texture *dest, Texture *src)
	{
		// Make sure that all of the properties match.
		if (dest->Width != src->Width) return false;
		if (dest->Height != src->Height) return false;
		if (dest->MipCount != src->MipCount) return false;
		if (dest->Format != src->Format) return false;
		if (dest->CubeMap != src->CubeMap) return false;
		dest->Transparent = src->Transparent;

		// They match, so we can copy the textures.
		Context->CopyResource(dest->ITexture, src->ITexture);
		return true;
	}

	inline bool ReadData(texture_data **texData)
	{
		// Verify that we can do this.
		if (!Output) return false;
		if (!texData) return false;

		// Populate texture data
		(*texData)->Format = formatConverter(Format);
		(*texData)->CubeMap = CubeMap;
		(*texData)->MipCount = MipCount;
		(*texData)->Mips = new mip_data[MipCount];
		(*texData)->Transparent = Transparent;
		for (unsigned i = 0; i < MipCount; i++)
		{
			(*texData)->Mips[i].Width = MipData[i].Width;
			(*texData)->Mips[i].Height = MipData[i].Height;
			(*texData)->Mips[i].BytesPerRow = MipData[i].BytesPerRow;
			(*texData)->Mips[i].DataSize = MipData[i].DataSize;
		}

		// Populate Texture Data.
		(*texData)->Data = new unsigned char *[CubeMap ? MipCount * 6 : MipCount];

		// Map the data.
		D3D11_MAPPED_SUBRESOURCE data;
		data.pData = nullptr;
		data.RowPitch = 0;
		data.DepthPitch = 0;

		// Map the texture data.
		unsigned maxCount = CubeMap ? MipCount * 6 : MipCount;
		for (unsigned i = 0, mip = 0, side = 0; i < maxCount; i++)
		{
			side = i % MipCount;
			mip = i - (side * MipCount);

			// Allocate memory.
			(*texData)->Data[i] = new unsigned char[MipData[mip].DataSize];

			// Begin read
			HRESULT result = Context->Map(ITexture, i, D3D11_MAP::D3D11_MAP_READ, NULL, &data);
			// If we fail, just zero-out the memory.
			if (FAILED(result)) ZeroMemory((*texData)->Data[i], MipData[mip].DataSize);
			// Otherwise, copy the data from the GPU.
			else
			{
				memcpy((*texData)->Data[i], data.pData, MipData[mip].DataSize);
				Context->Unmap(ITexture, i);
			}
		}
		return true;
	}

	texture_usage Usage = texture_usage::Undefined;

	ID3D11Texture2D *ITexture = nullptr;
	unsigned Width = 0;
	unsigned Height = 0;
	unsigned MipCount = 0;
	unsigned ArrayCount = 0;
	DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	bool CubeMap = false;
	bool Output = false;
	bool Transparent = false;
	mip_data *MipData = nullptr;
};

class RenderTargetView
{
public:
	enum class clear_mode
	{
		None,
		DepthOnly,
		StencilOnly,
		DepthAndStencil
	};

	inline RenderTargetView(std::shared_ptr<Texture> &texture, float clearColor[4], bool useDepthBuffer)
	{
		RenderTarget = texture;

		// Whether a depth buffer is in use.
		UseDepthBuffer = useDepthBuffer;

		// Set clear color
		ClearColor[0] = clearColor[0];
		ClearColor[1] = clearColor[1];
		ClearColor[2] = clearColor[2];
		ClearColor[3] = clearColor[3];

		// Set textures
		if (UseDepthBuffer)
		{
			DepthStencil = std::make_shared<Texture>(texture->Width, texture->Height, texture_format::D24_S8, false, false, texture_usage::DepthStencil);

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = NULL;
			dsvDesc.Texture2D.MipSlice = 0;
			Device->CreateDepthStencilView(DepthStencil->ITexture, &dsvDesc, &DepthStencilView);
		}

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = texture->Format;

		// Set View Count
		if (texture->CubeMap)
		{
			ViewCount = 6;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.ArraySize = 1;
		}
		else
		{
			ViewCount = 1;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
		}
		ViewList.resize(ViewCount);

		// Create render target views for the cube map.
		if (texture->CubeMap)
		{
			for (unsigned i = 0; i < ViewCount; i++)
			{
				rtvDesc.Texture2DArray.FirstArraySlice = i;
				Device->CreateRenderTargetView(texture->ITexture, &rtvDesc, &ViewList[i]);
			}
		}
		// Create render target view for non cube map.
		else Device->CreateRenderTargetView(texture->ITexture, &rtvDesc, ViewList.data());

		// Set viewport data
		Viewport.TopLeftX = 0.0f;
		Viewport.TopLeftY = 0.0f;
		Viewport.Width = (float)texture->Width;
		Viewport.Height = (float)texture->Height;
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;


	}
	inline virtual ~RenderTargetView()
	{
		// Deallocate depth stencil
		if (DepthStencilView) DepthStencilView->Release();

		// Deallocate target view
		for (unsigned i = 0; i < ViewCount; i++)
			ViewList[i]->Release();
	}

	inline void Set(unsigned index)
	{
		Context->RSSetViewports(1, &Viewport);
		if (index < ViewCount)
			Context->OMSetRenderTargets(1, &ViewList[index], DepthStencilView);
	}

	inline void Clear(bool clearTarget, clear_mode clearMode)
	{
		// Clear render targets.
		if (clearTarget)
		{
			for (unsigned i = 0; i < ViewCount; i++)
				Context->ClearRenderTargetView(ViewList[i], ClearColor);
		}

		// Clear depth and/or stencil
		switch (clearMode)
		{
		case clear_mode::DepthOnly:
			Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);
			break;
		case clear_mode::StencilOnly:
			Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
			break;
		case clear_mode::DepthAndStencil:
			Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
			break;
		}
	}
	inline void Resize(unsigned width, unsigned height)
	{

	}

	// For cube maps, there are 6 views (1 for each side).  For normal textures, there is just one view.
	unsigned ViewCount;
	bool UseDepthBuffer;
	float ClearColor[4];
	std::shared_ptr<Texture> RenderTarget;
	std::shared_ptr<Texture> DepthStencil;
	std::vector<ID3D11RenderTargetView *> ViewList;
	ID3D11DepthStencilView *DepthStencilView = nullptr;
	D3D11_VIEWPORT Viewport;
};

class ShaderResourceView
{
public:
	inline ShaderResourceView(std::shared_ptr<Texture> &texture)
	{
		ITexture = texture;
		IShaderResourceView = nullptr;	// This line is just to shut-up the VS warning.

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = texture->Format;
		if (texture->CubeMap)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = 6;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = -1;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
		}
		Device->CreateShaderResourceView(texture->ITexture, &srvDesc, &IShaderResourceView);
	}

	inline ~ShaderResourceView() { IShaderResourceView->Release(); }

	inline void Set(unsigned index) { Context->PSSetShaderResources(index, 1, &IShaderResourceView); }

	std::shared_ptr<Texture> ITexture;
	ID3D11ShaderResourceView *IShaderResourceView;
};

class ShaderResourceArray
{
public:
	inline ShaderResourceArray(size_t shaderResourceCount, std::shared_ptr<ShaderResourceView> *shaderResourceList)
	{
		_ShaderResources.reserve(shaderResourceCount);
		_IShaderResources.reserve(shaderResourceCount);
		for (size_t i = 0; i < shaderResourceCount; i++)
		{
			_ShaderResources[i] = shaderResourceList[i];
			if (_ShaderResources[i])
			{
				_IShaderResources[i] = _ShaderResources[i]->IShaderResourceView;
				_IShaderResources[i]->AddRef();
			}
		}
	}

	inline ~ShaderResourceArray()
	{
		for (size_t i = 0; i < _IShaderResources.size(); i++)
		{
			if (_IShaderResources[i])
				_IShaderResources[i]->Release();
		}
	}

	inline bool ReplaceShaderResource(size_t index, std::shared_ptr<ShaderResourceView> &shaderResource)
	{
		if (index >= _ShaderResources.size()) return false;
		_IShaderResources[index]->Release();
		_ShaderResources[index] = shaderResource;
		_IShaderResources[index] = shaderResource->IShaderResourceView;
		_IShaderResources[index]->AddRef();
		return true;
	}

	inline void Set() { Context->PSSetShaderResources(0, (unsigned)_IShaderResources.size(), _IShaderResources.data()); }

	std::vector<std::shared_ptr<ShaderResourceView>> _ShaderResources;
	std::vector<ID3D11ShaderResourceView *> _IShaderResources;
};

class Window;
class SwapChain
{
public:
	inline SwapChain(unsigned width, unsigned height, HWND hWnd, float clearColor[4], bool windowed)
	{
		HWnd = hWnd;
		Width = width;
		Height = height;
		Windowed = windowed;

		memcpy(ClearColor, clearColor, 16);

		// Swap chain description.
		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		scDesc.BufferDesc.Width = width;
		scDesc.BufferDesc.Height = height;
		scDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.BufferDesc.RefreshRate.Numerator = 0;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = 1;
		scDesc.OutputWindow = hWnd;
		scDesc.Windowed = windowed;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		scDesc.Flags = NULL;

		// Create the swap chain.
		IDXGIFactory *factory;
		CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
		factory->CreateSwapChain(Device, &scDesc, &ISwapChain);
		factory->Release();

		// Finally, create the render target.
		CreateRenderTarget();
	}

	inline ~SwapChain() { ISwapChain->Release(); }
	inline void Set() { RenderTarget->Set(0); }
	inline void Present() { ISwapChain->Present(0, 0); }

	inline void Resize(unsigned width, unsigned height, bool windowed)
	{
		Context->OMSetRenderTargets(0, nullptr, nullptr);
		RenderTarget = nullptr;
		HRESULT test = ISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, NULL);
		CreateRenderTarget();
	}

	inline void SetClearColor(float clearColor[4])
	{
		memcpy(ClearColor, clearColor, 16);
		memcpy(RenderTarget->ClearColor, clearColor, 16);
	}

	inline void Clear(bool clearTarget, RenderTargetView::clear_mode clearMode) { RenderTarget->Clear(clearTarget, clearMode); }

	inline void CreateRenderTarget()
	{
		// Get the back buffer and create the render target
		ID3D11Texture2D *backBuffer;
		ISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
		std::shared_ptr<Texture> bbTexture = std::make_shared<Texture>(backBuffer);	// bbTexture now owns back buffer pointer.
		backBuffer->Release();
		RenderTarget = std::make_shared<RenderTargetView>(bbTexture, ClearColor, true);	// RenderTarget now owns bbTexture
	}

	IDXGISwapChain *ISwapChain;
	std::shared_ptr<RenderTargetView> RenderTarget;
	HWND HWnd;

	float ClearColor[4];

	unsigned Width;
	unsigned Height;
	bool Windowed;
};

}}
