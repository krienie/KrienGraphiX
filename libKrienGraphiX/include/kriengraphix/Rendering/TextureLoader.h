
#pragma once
#include <string>

#include <windows.h>

struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace kgx
{
    namespace TextureLoader
    {
        HRESULT loadAsShaderResource( const std::wstring &file, ID3D11Resource **texture, ID3D11ShaderResourceView **shaderView,
                                      ID3D11Device *dxDev, ID3D11DeviceContext *dxDevCont );
        void closeThread();
    }
}

