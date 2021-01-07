#include "MathHelper.h"

using namespace DirectX;

DirectX::XMFLOAT4X4 MathHelper::Transpose(const DirectX::XMFLOAT4X4& mat)
{
    XMFLOAT4X4 result;
    XMStoreFloat4x4(&result, XMMatrixTranspose(XMLoadFloat4x4(&mat)));
    return result;
}
