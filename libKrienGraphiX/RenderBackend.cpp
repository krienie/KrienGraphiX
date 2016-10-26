
#define NOMINMAX

#include <algorithm>

#include "KGXCore.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "RenderCommand.h"
#include "RenderBackend.h"


namespace
{
	ID3D11DeviceContext *dxDevCont = nullptr;

	UINT numSRVBoundVS = 0u;
	UINT numSRVBoundPS = 0u;
}

namespace kgx
{
	namespace renderbackend
	{
		void bindMeshToIAStage( const void *data )
		{
			// extract command data
			const rendercommand::BindMeshToIAStage* realData = reinterpret_cast<const rendercommand::BindMeshToIAStage*>(data);

			MeshBuffer buff = ResourceManager::getInst()->getMeshBuffer( realData->meshBuffer );

			// bind Vertex- and Index-buffers to IA Stage
			UINT bufferStride = buff.vertexBufferStride;
			UINT offset       = 0;
			dxDevCont->IASetVertexBuffers( 0, 1, &buff.vertBuff, &bufferStride, &offset );
			dxDevCont->IASetIndexBuffer( buff.indexBuff, DXGI_FORMAT_R32_UINT, 0 );
			dxDevCont->IASetPrimitiveTopology( realData->primitiveTopology );
		}

		void drawIndexed( const void *data )
		{
			// extract command data
			const rendercommand::DrawIndexed* realData = reinterpret_cast<const rendercommand::DrawIndexed*>(data);

			dxDevCont->DrawIndexed( realData->indexCount, realData->startIndex, realData->baseVertex );
		}

		void copyConstantBufferData( const void *data )
		{
			// extract command data
			const rendercommand::CopyConstantBufferData* realData = reinterpret_cast<const rendercommand::CopyConstantBufferData*>(data);

			realData->constantBuffer->copyBufferData( realData->data, realData->size );
		}

		void updateConstantBufferVariable( const void *data )
		{
			// extract command data
			const rendercommand::UpdateConstantBufferVariable* realData = reinterpret_cast<const rendercommand::UpdateConstantBufferVariable*>(data);

			realData->constantBuffer->updateVariable( realData->name, realData->data );
		}

		void setVertexShaderSamplers( const void *data )
		{
			// extract command data
			const rendercommand::SetVertexShaderSamplers* realData = reinterpret_cast<const rendercommand::SetVertexShaderSamplers*>(data);

			dxDevCont->VSSetSamplers( realData->startSlot, realData->numSamplers, realData->samplers );
		}
		void setVertexShaderResourceViews( const void *data )
		{
			// extract command data
			const rendercommand::SetVertexShaderResourceViews* realData = reinterpret_cast<const rendercommand::SetVertexShaderResourceViews*>(data);

			dxDevCont->VSSetShaderResources( realData->startSlot, realData->numViews, realData->views );

			numSRVBoundVS = std::max(realData->numViews, numSRVBoundVS);
		}

		void setPixelShaderSamplers( const void *data )
		{
			// extract command data
			const rendercommand::SetPixelShaderSamplers* realData = reinterpret_cast<const rendercommand::SetPixelShaderSamplers*>(data);

			dxDevCont->PSSetSamplers( realData->startSlot, realData->numSamplers, realData->samplers );			
		}
		void setPixelShaderResourceViews( const void *data )
		{
			// extract command data
			const rendercommand::SetPixelShaderResourceViews* realData = reinterpret_cast<const rendercommand::SetPixelShaderResourceViews*>(data);

			dxDevCont->PSSetShaderResources( realData->startSlot, realData->numViews, realData->views );

			numSRVBoundPS = std::max(realData->numViews, numSRVBoundPS);
		}


		void resetState()
		{
			if ( numSRVBoundVS > 0u )
			{
				std::vector<ID3D11ShaderResourceView*> nullSRV( numSRVBoundVS, nullptr);
				dxDevCont->VSSetShaderResources( 0, numSRVBoundVS, nullSRV.data() );

				numSRVBoundVS = 0u;
			}

			if ( numSRVBoundPS > 0u )
			{
				std::vector<ID3D11ShaderResourceView*> nullSRV( numSRVBoundPS, nullptr);
				dxDevCont->PSSetShaderResources( 0, numSRVBoundPS, nullSRV.data() );

				numSRVBoundPS = 0u;
			}
		}

		void setDeviceContext( ID3D11Device *dxDevice )
		{
			releaseDeviceContext();

			if ( dxDevice )
				dxDevice->GetImmediateContext( &dxDevCont );
		}
		void releaseDeviceContext()
		{
			if ( dxDevCont )
			{
				dxDevCont->Release();
				dxDevCont = nullptr;
			}
		}
	}
}

