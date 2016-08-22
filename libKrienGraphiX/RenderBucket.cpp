
#define NOMINMAX
#include <limits>

#include "KGXCore.h"
#include "ResourceManager.h"

#include "RenderBucket.h"

namespace kgx
{
	RenderBucket::RenderBucket( ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv,
								const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projMatrix,
								unsigned int maxCommands )
		: m_keys(), m_commandPackets(), m_renderTargetView(rtv), m_depthStencilView(dsv),
			m_viewMatrix(viewMatrix), m_projMatrix(projMatrix)
	{
		ID3D11Device *dxDev = KGXCore::getInst()->getDxDevicePtr();
		dxDev->GetImmediateContext( &m_dxDevCont );

		m_keys.reserve(maxCommands);
		m_commandPackets.reserve( maxCommands );
	}

	RenderBucket::~RenderBucket()
	{
		if ( m_dxDevCont )
		{
			m_dxDevCont->Release();
			m_dxDevCont = nullptr;
		}

		// delete commands + all appended commands
		std::vector<RenderCommandPacket>::iterator it;
		for ( it = m_commandPackets.begin(); it != m_commandPackets.end(); ++it )
		{
			RenderCommandPacket packet = *it;
			do
			{
				RenderCommandPacket curPacket = packet;
				packet = rendercommandpacket::loadNextCommandPacket( packet );
				if ( curPacket )
					delete curPacket;
			} while ( packet != nullptr );
		}
	}

	void RenderBucket::sort()
	{
		//TODO: implement
	}

	void RenderBucket::submit( const LightData &lightData )
	{
		//TODO: let this be handled by the RenderBackend
		m_dxDevCont->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilView );

		UINT prevKey = std::numeric_limits<UINT>::max();	// this expects that there is at least one command/key pair added to the RenderBucket
		for ( UINT i = 0u; i < m_keys.size(); ++i )
		{
			//TODO: decode command key => activate new shaderprogram if necessary (also update the SP constants: camera data, lights, materials)

			if ( m_keys[i] != prevKey )
				decodeKey( m_keys[i], lightData );
			prevKey = m_keys[i];

			// process all commands for this key
			RenderCommandPacket packet = m_commandPackets[i];
			do
			{
				submitPacket( packet );
				packet = rendercommandpacket::loadNextCommandPacket( packet );
			} while ( packet != nullptr );
		}
	}

	void RenderBucket::submitPacket( const RenderCommandPacket packet )
	{
		const BackendDispatchFunction function = rendercommandpacket::loadBackendDispatchFunction( packet );
		const void* command = rendercommandpacket::loadCommand( packet );
		function( command );
	}

	void RenderBucket::decodeKey( UINT key, const LightData &lightData )
	{
		UINT shaderProgKey = (key >> 16) & 0xFFFF;
		UINT materialKey   = key & 0xFFFF;		// not used for now

		// for now, command keys are shader program ID's
		kgx::ShaderProgram *shaderProg = ResourceManager::getInst()->getShaderProgram( shaderProgKey );		//TODO: keys are composed of two ID's
		shaderProg->activate();		// bind shaders to pipeline

		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Vertex, "kgx_viewMatrix", &(m_viewMatrix.m[0]) );
		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Vertex, "kgx_projMatrix", &(m_projMatrix.m[0]) );
		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "ambLightClr", &lightData.ambientLight );
		shaderProg->updateShaderVar( ShaderProgram::ShaderType::Pixel, "sunLight", lightData.lights.data() );

		shaderProg->commitAllChanges();
	}
}
