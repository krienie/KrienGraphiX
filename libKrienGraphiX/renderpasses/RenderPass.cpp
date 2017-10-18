
#include <d3d11.h>

#include "../KGXCore.h"
#include "RenderPass.h"

namespace kgx
{
	RenderPass::RenderPass( ID3D11DeviceContext *deferredDevCont )
		: m_dxDevCont(nullptr), m_dxDeferredDevCont(deferredDevCont), m_commandList(nullptr)
	{
		ID3D11Device *dxDev = KGXCore::getInst()->getDxDevicePtr();
		dxDev->GetImmediateContext( &m_dxDevCont );
	}

	RenderPass::~RenderPass()
	{
		if ( m_dxDevCont )
			m_dxDevCont->Release();
		if ( m_commandList )
			m_commandList->Release();
	}

	void RenderPass::submit() const
	{
		if ( m_commandList )
			m_dxDevCont->ExecuteCommandList( m_commandList, false );
	}

	ID3D11DeviceContext * RenderPass::getDeferredContext() const
	{
		return m_dxDeferredDevCont;
	}

	void RenderPass::finishCommandList()
	{
		if ( m_commandList )
		{
			m_commandList->Release();
		}	m_commandList = nullptr;

		m_dxDeferredDevCont->FinishCommandList( false, &m_commandList );
		m_dxDeferredDevCont->ClearState();
	}
}
