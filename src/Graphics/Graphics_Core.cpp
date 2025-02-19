#include "Graphics_Core.h"

namespace TChapman500 {
namespace Graphics {

ID3D11Device *Device = nullptr;
ID3D11DeviceContext *Context = nullptr;

D3D11_BUFFER_DESC BufferDesc;
D3D11_MAPPED_SUBRESOURCE MappedBufferSubresource;

}}