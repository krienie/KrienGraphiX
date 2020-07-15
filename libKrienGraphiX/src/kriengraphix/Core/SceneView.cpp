
#include "Core/SceneView.h"

#include <iostream>

#include "Core/RenderWindow.h"
#include "Simulation/CameraComponent.h"
#include "Core/KGXCore.h"

namespace kgx
{
SceneView::SceneView(RenderWindow *parentWindow)
    : m_parentWindow(parentWindow), m_dxDevCont(nullptr),
        m_dxViewport(), m_rasterizer(nullptr)
{
    ID3D11Device *dxDev = KGXCore::get()->getDxDevicePtr();
    dxDev->GetImmediateContext(&m_dxDevCont);

    // Setup the rasterizer
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;      //D3D11_CULL_FRONT, D3D11_CULL_BACK or D3D11_CULL_NONE
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;     //D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = true;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    const HRESULT res = dxDev->CreateRasterizerState(&rasterDesc, &m_rasterizer);
    if (FAILED(res))
    {
        std::cout << "Error (SceneView::SceneView): Error creating Rasterizer state." << std::endl;
    }

    setViewport();
}

SceneView::~SceneView()
{
    if (m_rasterizer)
    {
        m_rasterizer->Release();
    }

    m_dxDevCont->Release();
}

D3D11_VIEWPORT SceneView::getViewport() const
{
    return m_dxViewport;
}

void SceneView::setViewport(float topLeftX, float topLeftY, float width, float height)
{
    // initialize viewport
    ZeroMemory(&m_dxViewport, sizeof(D3D11_VIEWPORT));

    if (topLeftX + width <= D3D11_VIEWPORT_BOUNDS_MAX)
    {
        m_dxViewport.TopLeftX = topLeftX;

        if (width < 0.0f)
        {
            m_dxViewport.Width = static_cast<float>(m_parentWindow->getWidth());
        } else
        {
            m_dxViewport.Width = width;
        }
    } else
    {
        std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Clamping viewport." << std::endl;
        m_dxViewport.TopLeftX = 0.0f;
        m_dxViewport.Width = D3D11_VIEWPORT_BOUNDS_MAX;
    }

    if (topLeftY + height <= D3D11_VIEWPORT_BOUNDS_MAX)
    {
        m_dxViewport.TopLeftY = topLeftY;

        if (height < 0.0f)
        {
            m_dxViewport.Height = static_cast<float>(m_parentWindow->getHeight());
        } else
        {
            m_dxViewport.Height = height;
        }
    } else
    {
        std::cout << "Warning (RenderWindow::addViewport): Viewport out of bounds. Clamping viewport." << std::endl;
        m_dxViewport.TopLeftY = 0.0f;
        m_dxViewport.Height = D3D11_VIEWPORT_BOUNDS_MAX;
    }

    m_dxViewport.MinDepth = D3D11_MIN_DEPTH;
    m_dxViewport.MaxDepth = D3D11_MAX_DEPTH;
}

ID3D11RenderTargetView *SceneView::getRenderTarget() const
{
    return m_parentWindow->getRTV();
}

ID3D11DepthStencilView *SceneView::getDepthStencil() const
{
    return m_parentWindow->getDSV();
}

ID3D11RasterizerState *SceneView::getRasterizerState() const
{
    return m_rasterizer;
}

float SceneView::getWidth() const
{
    return m_dxViewport.Width;
}

float SceneView::getHeight() const
{
    return m_dxViewport.Height;
}

void SceneView::clear()
{
    m_parentWindow->clear();
}

void SceneView::present()
{
    m_parentWindow->present();
}
}
