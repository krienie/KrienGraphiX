
/**
 * Inspired by Stefan Reinalter's blog on stateless, layered, multi-threaded rendering.
 * https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
 */

#pragma once

#include <d3d11.h>
#include <vector>

#include "RenderCommandPacket.h"

namespace kgx
{
	class Camera;

	class RenderBucket
	{
		public:
			RenderBucket( ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv,
							const DirectX::XMFLOAT4X4 &viewMatrix, const DirectX::XMFLOAT4X4 &projMatrix,
							unsigned int maxCommands = 512u );
			~RenderBucket();

			template<typename C>
			C* addCommand( UINT key, size_t auxMemorySize )
			{
				RenderCommandPacket packet = rendercommandpacket::create<C>( auxMemorySize );

				m_keys.push_back( key );
				m_commandPackets.push_back( packet );

				rendercommandpacket::storeNextCommandPacket( packet, nullptr );
				rendercommandpacket::storeBackendDispatchFunction( packet, C::DISPATCH_FUNCTION );

				return rendercommandpacket::getCommand<C>( packet );
			}

			template <typename U, typename V>
			U* appendCommand( V* command, size_t auxMemorySize )
			{
				RenderCommandPacket packet = rendercommandpacket::create<U>( auxMemorySize );

				// append this command to the given one
				rendercommandpacket::storeNextCommandPacket<V>( command, packet );

				rendercommandpacket::storeNextCommandPacket( packet, nullptr );
				rendercommandpacket::storeBackendDispatchFunction( packet, U::DISPATCH_FUNCTION );

				return rendercommandpacket::getCommand<U>( packet );
			}

			void sort();
			void submit( const LightData &lightData );

		private:
			void submitPacket( const RenderCommandPacket packet );
			void decodeKey( UINT key, const LightData &lightData );

			std::vector<UINT> m_keys;
			std::vector<RenderCommandPacket> m_commandPackets;

			ID3D11DeviceContext *m_dxDevCont;
			ID3D11RenderTargetView *m_renderTargetView;
			ID3D11DepthStencilView *m_depthStencilView;
			DirectX::XMFLOAT4X4 m_viewMatrix;
			DirectX::XMFLOAT4X4 m_projMatrix;
	};
}
