#pragma once
#include "Graphics.h"

namespace TChapman500 {
namespace Graphics {

inline bool CreateGraphics()
{
	// Clear descriptors to zero.
	memset(&BufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	memset(&MappedBufferSubresource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Initialize Device and Context
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &Device, nullptr, &Context);
	if (FAILED(result)) return false;
	return true;
}

inline void DestroyGraphics()
{
	Context->Release();
	Device->Release();
}

inline void Draw(unsigned vtxCount) { Context->Draw(vtxCount, 0); }
inline void Draw(unsigned startVtx, unsigned vtxCount) { Context->Draw(vtxCount, startVtx); }
inline void DrawInstanced(unsigned vtxCount, unsigned instCount) { Context->DrawInstanced(vtxCount, instCount, 0, 0); }
inline void DrawInstanced(unsigned startVtx, unsigned vtxCount, unsigned startInst, unsigned instCount) { Context->DrawInstanced(vtxCount, instCount, startVtx, startInst); }
inline void DrawIndexed(unsigned idxCount) { Context->DrawIndexed(idxCount, 0, 0); }
inline void DrawIndexed(unsigned startIdx, unsigned idxCount) { Context->DrawIndexed(idxCount, startIdx, 0); }
inline void DrawIndexedInstanced(unsigned idxCount, unsigned instCount) { Context->DrawIndexedInstanced(idxCount, instCount, 0, 0, 0); }
inline void DrawIndexedInstanced(unsigned startIdx, unsigned idxCount, unsigned startInst, unsigned instCount) { Context->DrawIndexedInstanced(idxCount, instCount, startIdx, 0, startInst); }

inline void SetTopology(topology topology)
{
	const D3D11_PRIMITIVE_TOPOLOGY topologies[] = {
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	Context->IASetPrimitiveTopology(topologies[(size_t)topology]);
}

}}
