
/**
 * Inspired by Stefan Reinalter's blog on stateless, layered, multi-threaded rendering.
 * https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
 */

#pragma once

#include <d3d11.h>

#include "Defines.h"
#include "RenderBackend.h"

namespace kgx
{
	class Camera;
	class ConstantBuffer;
	class ShaderProgram;

	namespace rendercommand
	{
		struct BindMeshToIAStage
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
			MeshBufferID meshBuffer;
		};

		struct DrawIndexed
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			UINT indexCount;
			UINT startIndex;
			UINT baseVertex;
		};

		struct CopyConstantBufferData
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			ConstantBuffer *constantBuffer;
			UINT size;
			void *data;
		};

		//TODO: change this command to an explicit copy-only call that stores the data to be copied in the command's auxilary data
		struct UpdateConstantBufferVariable
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			ConstantBuffer *constantBuffer;
			std::string name;
			void *data;
		};

		struct SetVertexShaderSamplers
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			UINT startSlot;
			UINT numSamplers;
			ID3D11SamplerState **samplers;
		};

		struct SetPixelShaderSamplers
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			UINT startSlot;
			UINT numSamplers;
			ID3D11SamplerState **samplers;
		};

		struct SetVertexShaderResourceViews
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			UINT startSlot;
			UINT numViews;
			ID3D11ShaderResourceView **views;
		};

		struct SetPixelShaderResourceViews
		{
			static const BackendDispatchFunction DISPATCH_FUNCTION;

			UINT startSlot;
			UINT numViews;
			ID3D11ShaderResourceView **views;
		};
	};
}
