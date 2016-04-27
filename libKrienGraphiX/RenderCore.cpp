
#include <iostream>

#include "Camera.h"
#include "RenderCore.h"
#include "ShaderProgram.h"

namespace kgx
{
	RenderCore::RenderCore( ID3D11Device *dxDevice )
		: m_dxDev(dxDevice), m_dxDevCont(nullptr), m_depthStencilView(nullptr), m_rasterizer(nullptr),
		m_curShaderProg(nullptr)
	{
		m_dxDev->GetImmediateContext( &m_dxDevCont );
	}

	RenderCore::~RenderCore()
	{
		if ( m_rasterizer )
			m_rasterizer->Release();

		if ( m_depthStencilView )
			m_depthStencilView->Release();

		if ( m_dxDevCont )
			m_dxDevCont->Release();
	}


	ID3D11DepthStencilView* RenderCore::getDepthStencilView() const
	{
		return m_depthStencilView;
	}

	void RenderCore::setShaderProgram( ShaderProgram *shaderProg )
	{
		m_curShaderProg = shaderProg;
	}


	bool RenderCore::init( UINT depthBuffWidth, UINT depthBuffHeight, UINT sampleCount, UINT sampleQuality )
	{
		// create and initialize depth-stencil buffer
		ID3D11Texture2D *depthBuffTex = nullptr;
		D3D11_TEXTURE2D_DESC depthBuffDesc;
		depthBuffDesc.Width              = depthBuffWidth;
		depthBuffDesc.Height             = depthBuffHeight;
		depthBuffDesc.MipLevels          = 1;
		depthBuffDesc.ArraySize          = 1;
		depthBuffDesc.Format             = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBuffDesc.Usage              = D3D11_USAGE_DEFAULT;
		depthBuffDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		depthBuffDesc.CPUAccessFlags     = 0;
		depthBuffDesc.SampleDesc.Count   = sampleCount;
		depthBuffDesc.SampleDesc.Quality = sampleQuality;
		depthBuffDesc.MiscFlags          = 0;

		m_dxDev->CreateTexture2D( &depthBuffDesc, nullptr, &depthBuffTex );
		HRESULT res = m_dxDev->CreateDepthStencilView( depthBuffTex, nullptr, &m_depthStencilView );
		depthBuffTex->Release();

		if ( FAILED( res ) )
		{
			std::cout << "Error (RenderCore::init): Error creating DepthStencilView." << std::endl;
			return false;
		}


		// Setup the m_rasterizer
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode				 = D3D11_CULL_BACK;		//D3D11_CULL_FRONT, D3D11_CULL_BACK or D3D11_CULL_NONE
		rasterDesc.DepthBias			 = 0;
		rasterDesc.DepthBiasClamp		 = 0.0f;
		rasterDesc.DepthClipEnable		 = true;
		rasterDesc.FillMode				 = D3D11_FILL_SOLID;	//D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable	 = true;
		rasterDesc.ScissorEnable		 = false;
		rasterDesc.SlopeScaledDepthBias  = 0.0f;

		res = m_dxDev->CreateRasterizerState(&rasterDesc, &m_rasterizer);
		if ( FAILED(res) )
		{
			m_depthStencilView->Release();
			m_depthStencilView = nullptr;
			std::cout << "Error (RenderCore::init): Error creating Rasterizer state." << std::endl;
			return false;
		}

		return true;
	}

	void RenderCore::renderFrame( Camera *camera )
	{
		m_dxDevCont->RSSetState( m_rasterizer );

		// clear the depth-stencil buffer
		m_dxDevCont->ClearDepthStencilView( m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

		//TODO: get the correct shaders here

		if ( m_curShaderProg )
			camera->renderCurrentView(m_curShaderProg);
	}
}
