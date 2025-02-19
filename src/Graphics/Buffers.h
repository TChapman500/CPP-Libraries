#pragma once
#include "Graphics.h"
#include <vector>
#include <memory>

namespace TChapman500 {
namespace Graphics {

inline D3D11_BUFFER_DESC *vtxBufferDesc()
{
	BufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.StructureByteStride = 0;
	return &BufferDesc;
}
inline D3D11_BUFFER_DESC *idxBufferDesc()
{
	BufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	BufferDesc.StructureByteStride = 0;
	return &BufferDesc;
}
inline D3D11_BUFFER_DESC *constBufferDesc()
{
	BufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.StructureByteStride = 0;
	return &BufferDesc;
}

class Buffer
{
public:
	inline Buffer(unsigned size, void *initData, D3D11_BUFFER_DESC *desc, bool readOnly)
	{
		// Store Buffer Properties.
		Size = size;
		ReadOnly = readOnly;


		// Ensure that the description is set correctly.
		desc->ByteWidth = size;
		desc->MiscFlags = NULL;

		// Apply read-only attributes.
		if (readOnly)
		{
			desc->Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
			desc->CPUAccessFlags = NULL;
		}
		// Apply read/write attributes.
		else
		{
			desc->Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			desc->CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		}

		// Create buffer with initial data.
		if (initData)
		{
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = initData;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			Device->CreateBuffer(desc, &data, &IBuffer);
		}
		// Create buffer without initialization.
		else Device->CreateBuffer(desc, nullptr, &IBuffer);
	}

	inline virtual ~Buffer() { IBuffer->Release(); }

	inline bool WriteBuffer(void *data) { return WriteBuffer(data, Size); }

	inline bool WriteBuffer(void *data, size_t sizeLimit)
	{
		// Cannot write to a read-only buffer.
		if (sizeLimit > Size || ReadOnly) return false;

		// Attempt to write to buffer.
		HRESULT result = Context->Map(IBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &MappedBufferSubresource);
		if (FAILED(result)) return false;
		memcpy(MappedBufferSubresource.pData, data, sizeLimit);
		Context->Unmap(IBuffer, 0);
		return true;
	}

	ID3D11Buffer *IBuffer;
	unsigned Size;
	bool ReadOnly;
};

class VertexBuffer : public Buffer
{
public:
	inline VertexBuffer(unsigned floatsPerVertex, unsigned vertexCount, float *initData, bool readOnly) : Buffer(4 * floatsPerVertex * vertexCount, initData, vtxBufferDesc(), readOnly)
	{
		VertexCount = vertexCount;
		FloatsPerVertex = floatsPerVertex;

		Stride = FloatsPerVertex * 4;
		Offset = 0;
	}

	inline ~VertexBuffer() {}

	inline void Set(unsigned index) { Context->IASetVertexBuffers(index, 1, &IBuffer, &Stride, &Offset); }

	unsigned FloatsPerVertex;
	unsigned VertexCount;

	unsigned Stride;
	unsigned Offset;
};

class IndexBuffer : public Buffer
{
public:
	inline IndexBuffer(unsigned indexCount, unsigned *initData, bool readOnly) : Buffer(4 * indexCount, initData, idxBufferDesc(), readOnly) { IndexCount = indexCount; }
	inline ~IndexBuffer() {}

	inline void Set(unsigned index) { Context->IASetIndexBuffer(IBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0); }

	unsigned IndexCount;
};

class ConstantBuffer : public Buffer
{
public:
	enum class buffer_shader
	{
		VertexShader,
		PixelShader,
		ComputeShader
	};

	inline ConstantBuffer(unsigned rowCount, float *initData, buffer_shader bufferShader) : Buffer(16 * rowCount, initData, constBufferDesc(), false)
	{
		RowCount = rowCount;
		BufferShader = bufferShader;
	}
	inline ~ConstantBuffer() {}

	inline void Set(unsigned index)
	{
		switch (BufferShader)
		{
		case buffer_shader::VertexShader:
			Context->VSSetConstantBuffers(index, 1, &IBuffer);
			break;
		case buffer_shader::PixelShader:
			Context->PSSetConstantBuffers(index, 1, &IBuffer);
			break;
		case buffer_shader::ComputeShader:
			Context->CSSetConstantBuffers(index, 1, &IBuffer);
			break;
		}
	}

	buffer_shader BufferShader;
	unsigned RowCount;
};

class BufferArray
{
public:
	inline BufferArray(size_t bufferCount, std::shared_ptr<Buffer> *bufferList)
	{
		_Buffers.reserve(bufferCount);
		_IBuffers.reserve(bufferCount);
		for (size_t i = 0; i < bufferCount; i++)
		{
			_Buffers[i] = bufferList[i];
			_IBuffers[i] = bufferList[i]->IBuffer;
			_IBuffers[i]->AddRef();
		}
	}

	inline ~BufferArray()
	{
		for (size_t i = 0; i < _IBuffers.size(); i++)
			_IBuffers[i]->Release();
	}

	inline bool ReplaceBuffer(size_t index, std::shared_ptr<Buffer> &buffer)
	{
		if (index >= _Buffers.size()) return false;
		_IBuffers[index]->Release();
		_Buffers[index] = buffer;
		_IBuffers[index] = buffer->IBuffer;
		_IBuffers[index]->AddRef();
		return true;
	}

	std::vector<std::shared_ptr<Buffer>> _Buffers;
	std::vector<ID3D11Buffer *> _IBuffers;
};

class VertexBufferArray : BufferArray
{
public:
	inline VertexBufferArray(size_t bufferCount, std::shared_ptr<Buffer> *bufferList) : BufferArray(bufferCount, bufferList)
	{
		_Strides.reserve(bufferCount);
		_Offsets.resize(bufferCount);
		for (size_t i = 0; i < bufferCount; i++)
		{
			VertexBuffer *buffer = (VertexBuffer *)bufferList[i].get();
			_Strides[i] = buffer->Stride;
		}
	}
	inline ~VertexBufferArray() {}

	inline bool ReplaceBuffer(size_t index, std::shared_ptr<Buffer> &buffer)
	{
		if (BufferArray::ReplaceBuffer(index, buffer))
		{
			VertexBuffer *vBuffer = (VertexBuffer *)buffer.get();
			_Strides[index] = vBuffer->Stride;
			return true;
		}
		return false;
	}

	inline void Set() { Context->IASetVertexBuffers(0, (unsigned)_Buffers.size(), _IBuffers.data(), _Strides.data(), _Offsets.data()); }

	std::vector<unsigned> _Strides;
	std::vector<unsigned> _Offsets;
};

}}
