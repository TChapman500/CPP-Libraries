#pragma once
#include "Buffers.h"
#include <d3dcompiler.h>
#include <memory>
#include <vector>
#include <string>

namespace TChapman500 {
namespace Graphics {

class InputLayout;
class VertexShader;

struct shader_create_params
{
	std::wstring Path;
	std::string Entry;
	std::vector<std::shared_ptr<ConstantBuffer>> Buffers;
	std::vector<size_t> BufferRows;
};

class InputLayout
{
public:
	inline InputLayout(size_t elementCount, il_create_param *elements, VertexShader &vs);

	inline ~InputLayout();

	inline void Set();

	ID3D11InputLayout *IInputLayout;
};

class Shader
{
public:
	inline Shader(shader_create_params &params, std::string target)
	{
		HRESULT result = D3DCompileFromFile(params.Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, params.Entry.c_str(), target.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, NULL, &Bytecode, &ErrorMessage);
		if (FAILED(result))
		{
			size_t msgLen = ErrorMessage->GetBufferSize();
			char *msg = (char *)ErrorMessage->GetBufferPointer();
			int error = 1;
		}

		bool error = false;
		if (!params.BufferRows.size())
			goto SHADER_CREATE_ERROR;
		if (params.Buffers.size() != 0 && params.Buffers.size() != params.BufferRows.size())
			goto SHADER_CREATE_ERROR;

		RawCBuffers.resize(params.BufferRows.size());
		CBuffers.resize(params.BufferRows.size());
		CBufferRows.resize(params.BufferRows.size());
		memcpy_s(CBufferRows.data(), CBufferRows.size() * sizeof(size_t), params.BufferRows.data(), params.BufferRows.size() * sizeof(size_t));

		for (size_t i = 0; i < CBuffers.size(); i++)
		{
			// Must not be zero!
			if (!params.BufferRows[i])
			{
				error = true;
				CBuffers[i] = nullptr;
				CBufferRows[i] = 0;
				continue;
			}

			CBufferRows[i] = params.BufferRows[i];
			CBuffers[i] = params.Buffers[i];

			if (params.Buffers[i])
			{
				RawCBuffers[i] = params.Buffers[i]->IBuffer;
				RawCBuffers[i]->AddRef();
			}
			else
			{
				CBuffers[i] = nullptr;
				RawCBuffers[i] = nullptr;
			}
		}

	SHADER_CREATE_ERROR:
		if (!error) return;
		for (size_t i = 0; i < CBuffers.size(); i++)
		{
			if (RawCBuffers[i])
				RawCBuffers[i]->Release();
		}
		CBuffers.clear();
		CBufferRows.clear();
		RawCBuffers.clear();
	}

	inline virtual ~Shader()
	{
		Bytecode->Release();
		if (ErrorMessage) ErrorMessage->Release();
		for (size_t i = 0; i < RawCBuffers.size(); i++)
		{
			if (RawCBuffers[i])
				RawCBuffers[i]->Release();
		}
	}

	ID3D10Blob *ErrorMessage;
	ID3D10Blob *Bytecode;
	std::vector<std::shared_ptr<ConstantBuffer>> CBuffers;
	std::vector<ID3D11Buffer *> RawCBuffers;
	std::vector<size_t> CBufferRows;
};

class VertexShader : public Shader
{
public:
	inline VertexShader(shader_create_params &params) : Shader(params, (char *)"vs_5_0")
	{
		Device->CreateVertexShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), nullptr, &IVertexShader);

		for (size_t i = 0; i < CBuffers.size(); i++)
		{
			if (CBuffers[i]) continue;
			CBuffers[i] = std::make_shared<ConstantBuffer>((unsigned)CBufferRows[i], nullptr, ConstantBuffer::buffer_shader::VertexShader);
			RawCBuffers[i] = CBuffers[i]->IBuffer;
			RawCBuffers[i]->AddRef();
		}

		_InputLayout = nullptr;
		IInputLayout = nullptr;
	}

	inline VertexShader(shader_create_params &params, std::shared_ptr<InputLayout> inputLayout) : VertexShader(params) { SetInputLayout(inputLayout); }

	inline ~VertexShader()
	{
		if (_InputLayout)
		{
			_InputLayout = nullptr;
			IInputLayout->Release();
		}
		IVertexShader->Release();
	}

	inline void Set()
	{
		Context->IASetInputLayout(IInputLayout);
		Context->VSSetShader(IVertexShader, nullptr, 0);
		Context->VSSetConstantBuffers(0, (unsigned)RawCBuffers.size(), RawCBuffers.data());
	}

	inline void SetInputLayout(std::shared_ptr<InputLayout> inputLayout)
	{
		if (_InputLayout) return;
		_InputLayout = inputLayout;
		if (!_InputLayout) return;
		IInputLayout = inputLayout->IInputLayout;
		IInputLayout->AddRef();
	}

	std::shared_ptr<InputLayout> _InputLayout;
	ID3D11VertexShader *IVertexShader;
	ID3D11InputLayout *IInputLayout;
};

class PixelShader : public Shader
{
public:
	inline PixelShader(shader_create_params &params) : Shader(params, (char *)"ps_5_0")
	{
		Device->CreatePixelShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), nullptr, &IPixelShader);

		for (size_t i = 0; i < CBuffers.size(); i++)
		{
			if (CBuffers[i]) continue;
			CBuffers[i] = std::make_shared<ConstantBuffer>((unsigned)CBufferRows[i], nullptr, ConstantBuffer::buffer_shader::PixelShader);
			RawCBuffers[i] = CBuffers[i]->IBuffer;
			RawCBuffers[i]->AddRef();
		}
	}

	inline ~PixelShader() { IPixelShader->Release(); }

	inline void Set()
	{
		Context->PSSetShader(IPixelShader, nullptr, 0);
		Context->PSSetConstantBuffers(0, (unsigned)RawCBuffers.size(), RawCBuffers.data());
	}

	ID3D11PixelShader *IPixelShader;
};

class ComputeShader : public Shader
{
public:
	inline ComputeShader(shader_create_params &params) : Shader(params, (char *)"cs_5_0")
	{
		Device->CreateComputeShader(Bytecode->GetBufferPointer(), Bytecode->GetBufferSize(), nullptr, &IComputeShader);

		for (size_t i = 0; i < CBuffers.size(); i++)
		{
			if (CBuffers[i]) continue;
			CBuffers[i] = std::make_shared<ConstantBuffer>((unsigned)CBufferRows[i], nullptr, ConstantBuffer::buffer_shader::ComputeShader);
			RawCBuffers[i] = CBuffers[i]->IBuffer;
			RawCBuffers[i]->AddRef();
		}
	}

	inline ~ComputeShader() { IComputeShader->Release(); }

	inline void Set() { Context->CSSetShader(IComputeShader, nullptr, 0); }

	ID3D11ComputeShader *IComputeShader;
};

InputLayout::InputLayout(size_t elementCount, il_create_param *elements, VertexShader &vs)
{
	IInputLayout = nullptr;

	// Validate elements
	if (!elementCount || !elements) return;
	for (size_t i = 0; i < elementCount; i++)
	{
		if (!elements[i].Semantic) return;
		if (elements[i].FloatCount < 1 || elements[i].FloatCount > 4) return;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
	desc.resize(elementCount);

	// Semantic Name, Semantic Index
	std::vector<std::pair<unsigned, unsigned>> slotElements;
	std::vector<std::pair<std::string, unsigned>> semantics;

	for (size_t i = 0; i < elementCount; i++)
	{
		// Semantic Name and Index
		desc[i].SemanticName = elements[i].Semantic;
		bool notFound = true;
		for (size_t s = 0; s < semantics.size(); s++)
		{
			if (semantics[s].first == elements[i].Semantic)
			{
				semantics[s].second++;
				desc[i].SemanticIndex = semantics[s].second;
				notFound = false;
				break;
			}
		}
		if (notFound)
		{
			semantics.push_back(std::pair<std::string, unsigned>(elements[i].Semantic, 0));
			desc[i].SemanticIndex = 0;
		}

		// Format
		switch (elements[i].FloatCount)
		{
		case 1:
			desc[i].Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			break;
		case 2:
			desc[i].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
			break;
		case 3:
			desc[i].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case 4:
			desc[i].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		}

		notFound = true;
		for (size_t s = 0; s < slotElements.size(); s++)
		{
			if (slotElements[s].first == elements[i].InputSlot)
			{
				desc[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				slotElements[s].second++;
				notFound = false;
				break;
			}
		}
		if (notFound)
		{
			slotElements.push_back(std::pair<unsigned, unsigned>(elements[i].InputSlot, 1));
			desc[i].AlignedByteOffset = 0;
		}

		// Instancing
		desc[i].InputSlotClass = elements[i].IsInstanced ? D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		desc[i].InstanceDataStepRate = elements[i].InstanceStepRate;
	}

	Device->CreateInputLayout(desc.data(), (UINT)elementCount, vs.Bytecode->GetBufferPointer(), (SIZE_T)vs.Bytecode->GetBufferSize(), &IInputLayout);
}

InputLayout::~InputLayout() { if (IInputLayout) IInputLayout->Release(); }

void InputLayout::Set() { Context->IASetInputLayout(IInputLayout); }

}
}
