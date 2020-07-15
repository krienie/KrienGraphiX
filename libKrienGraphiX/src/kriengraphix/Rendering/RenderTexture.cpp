
#include "Rendering/RenderTexture.h"

#include <d3d11.h>
#include "Core/KGXCore.h"
#include <iostream>

namespace kgx
{
RenderTexture::RenderTexture(TextureID id, unsigned int width, unsigned int height)
    : Texture(id, nullptr, nullptr), m_dxDev(nullptr),
        m_dxDevCont(nullptr), m_renderTargetView(nullptr), m_width(width), m_height(height)
{
    m_dxDev = KGXCore::get()->getDxDevicePtr();
    m_dxDev->GetImmediateContext(&m_dxDevCont);

    if (!createDxRenderTexture(width, height, &m_buffer, &m_shaderView, &m_renderTargetView))
    {
        std::cout << "Warning (RenderTexture::RenderTexture): Error creating render texture resource" << std::endl;
    }
}

RenderTexture::~RenderTexture()
{
    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
    }

    if (m_dxDevCont)
    {
        m_dxDevCont->Release();
    }
}

ID3D11RenderTargetView* RenderTexture::getRenderTargetView() const
{
    return m_renderTargetView;
}

unsigned int RenderTexture::getWidth() const
{
    return m_width;
}

unsigned int RenderTexture::getHeight() const
{
    return m_height;
}

bool RenderTexture::createDxRenderTexture(unsigned int width, unsigned int height, ID3D11Resource **buff, ID3D11ShaderResourceView **srv, ID3D11RenderTargetView **rtv)
{
    // Create texture
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0; //D3D11_STANDARD_MULTISAMPLE_PATTERN;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    ID3D11Texture2D *texture = nullptr;
    HRESULT res = m_dxDev->CreateTexture2D( &texDesc, nullptr, &texture );
    if (FAILED(res))
    {
        return false;
    }

    // Setup the description of the render target view.
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    // Create the render target view
    ID3D11RenderTargetView *renderTargetView;
    res = m_dxDev->CreateRenderTargetView( texture, &rtvDesc, &renderTargetView );
    if (FAILED(res))
    {
        texture->Release();
        return false;
    }

    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

    // Create the shader resource view.
    ID3D11ShaderResourceView *shaderResourceView;
    res = m_dxDev->CreateShaderResourceView( texture, &srvDesc, &shaderResourceView );
    if (FAILED(res))
    {
        renderTargetView->Release();
        texture->Release();
        return false;
    }

    *buff = texture;
    *srv  = shaderResourceView;
    *rtv  = renderTargetView;
    return true;
}
}
