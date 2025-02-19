#pragma once
#include "Graphics.h"
#include <vector>
#include <memory>

namespace TChapman500 {
namespace Graphics {

class Sampler
{
public:
	inline Sampler(sampler_mode modeU, sampler_mode modeV, sampler_filter filter, float mipBias)
	{
		float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		const D3D11_TEXTURE_ADDRESS_MODE addrMode[] = {
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_BORDER
		};
		const D3D11_FILTER dFilter[] = {
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC
		};
		const UINT anisotropy[] = { 1, 1, 1, 2, 4, 8, 16 };

		D3D11_SAMPLER_DESC desc;
		desc.AddressU = addrMode[(size_t)modeU];
		desc.AddressV = addrMode[(size_t)modeV];
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.Filter = dFilter[(size_t)filter];
		desc.MaxAnisotropy = anisotropy[(size_t)filter];
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = mipBias;
		memcpy_s(desc.BorderColor, sizeof(float) * 4, borderColor, sizeof(float) * 4);

		Device->CreateSamplerState(&desc, &ISamplerState);
	}

	inline Sampler(sampler_mode modeU, sampler_mode modeV, sampler_filter filter, float mipBias, float borderColor[4])
	{
		const D3D11_TEXTURE_ADDRESS_MODE addrMode[] = {
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_BORDER
		};
		const D3D11_FILTER dFilter[] = {
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC,
			D3D11_FILTER_ANISOTROPIC
		};
		const UINT anisotropy[] = { 1, 1, 1, 2, 4, 8, 16 };

		D3D11_SAMPLER_DESC desc;
		desc.AddressU = addrMode[(size_t)modeU];
		desc.AddressV = addrMode[(size_t)modeV];
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.Filter = dFilter[(size_t)filter];
		desc.MaxAnisotropy = anisotropy[(size_t)filter];
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MipLODBias = mipBias;
		memcpy_s(desc.BorderColor, sizeof(float) * 4, borderColor, sizeof(float) * 4);

		Device->CreateSamplerState(&desc, &ISamplerState);
	}

	inline ~Sampler() { ISamplerState->Release(); }

	inline void Set(unsigned index) { Context->PSSetSamplers(index, 1, &ISamplerState); }

	ID3D11SamplerState *ISamplerState;
};

class RasterizerState
{
public:
	inline RasterizerState(bool wireframe, int cullMode, int depthBias, float biasClamp, float scaledBiasClamp, bool enDepthClip, bool ccFrontFace, bool enScisser, bool enMultisample, bool endAntiAliasing)
	{
		D3D11_RASTERIZER_DESC desc;
		desc.AntialiasedLineEnable = endAntiAliasing;
		desc.DepthBias = depthBias;
		desc.DepthBiasClamp = biasClamp;
		desc.DepthClipEnable = enDepthClip;
		desc.FillMode = wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = ccFrontFace;
		desc.MultisampleEnable = enMultisample;
		desc.ScissorEnable = enScisser;
		desc.SlopeScaledDepthBias = scaledBiasClamp;

		if (cullMode < 0) desc.CullMode = D3D11_CULL_BACK;
		else if (cullMode > 0) desc.CullMode = D3D11_CULL_FRONT;
		else desc.CullMode = D3D11_CULL_NONE;

		Device->CreateRasterizerState(&desc, &IRasterizerState);
	}

	inline ~RasterizerState() { IRasterizerState->Release(); }

	inline void Set() { Context->RSSetState(IRasterizerState); }

	inline void SetViewport(viewport &viewport)
	{
		D3D11_VIEWPORT viewDesc;
		viewDesc.TopLeftX = viewport.Left;
		viewDesc.TopLeftY = viewport.Top;
		viewDesc.Width = viewport.Width;
		viewDesc.Height = viewport.Height;
		viewDesc.MinDepth = viewport.MinDepth;
		viewDesc.MaxDepth = viewport.MaxDepth;

		Context->RSSetViewports(1, &viewDesc);
	}

	ID3D11RasterizerState *IRasterizerState;
};

class BlendState
{
public:
	inline BlendState(bool transparent)
	{
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = transparent;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

		if (transparent)
		{
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		}
		else
		{
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		}

		Device->CreateBlendState(&desc, &IBlendState);
	}

	inline ~BlendState() { if (IBlendState) IBlendState->Release(); }

	inline void Set()
	{
		const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		Context->OMSetBlendState(IBlendState, blendFactor, 0xFFFFFFFF);
	}

	ID3D11BlendState *IBlendState;
};

class DepthStencilState
{
public:
	inline DepthStencilState()
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		Device->CreateDepthStencilState(&dsDesc, &IDepthStencilState);
	}

	inline DepthStencilState(bool depth, stencil_op stencilOp)
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		if (depth)
		{
			dsDesc.DepthEnable = TRUE;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		}

		if (stencilOp > stencil_op::Disable)
		{
			dsDesc.StencilEnable = TRUE;
			dsDesc.StencilReadMask = 0xFF;
			if (stencilOp == stencil_op::Write)
			{
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			}
			else
			{
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
				dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
				dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
			}
		}

		Device->CreateDepthStencilState(&dsDesc, &IDepthStencilState);
	}

	inline ~DepthStencilState() { if (IDepthStencilState) IDepthStencilState->Release(); }

	inline void Set() { Context->OMSetDepthStencilState(IDepthStencilState, 0); }

	inline void Set(unsigned stencilRef) { Context->OMSetDepthStencilState(IDepthStencilState, stencilRef); }

	ID3D11DepthStencilState *IDepthStencilState;
};

class SamplerArray
{
public:
	inline SamplerArray(size_t samplerCount, std::shared_ptr<Sampler> *samplerList)
	{
		_Samplers.reserve(samplerCount);
		_ISamplers.reserve(samplerCount);
		for (size_t i = 0; i < samplerCount; i++)
		{
			_Samplers[i] = samplerList[i];
			if (_Samplers[i])
			{
				_ISamplers[i] = _Samplers[i]->ISamplerState;
				_ISamplers[i]->AddRef();
			}
		}
	}

	inline ~SamplerArray()
	{
		for (size_t i = 0; i < _ISamplers.size(); i++)
		{
			if (_ISamplers[i])
				_ISamplers[i]->Release();
		}
	}

	inline bool ReplaceSampler(size_t index, std::shared_ptr<Sampler> &sampler)
	{
		if (index >= _Samplers.size()) return false;
		_ISamplers[index]->Release();
		_Samplers[index] = sampler;
		_ISamplers[index] = sampler->ISamplerState;
		_ISamplers[index]->AddRef();
		return true;
	}

	inline void Set() { Context->PSSetSamplers(0, (unsigned)_ISamplers.size(), _ISamplers.data()); }

	std::vector<std::shared_ptr<Sampler>> _Samplers;
	std::vector<ID3D11SamplerState *> _ISamplers;
};

}}
