
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Defines.h"


namespace kgx
{
	class Camera;
	class Material;

	//TODO: maybe rename this class to something more suitable => One RenderableObject can contain more than one object to render..
	class RenderableObject
	{
		public:
			struct Mesh
			{
				std::string name;
				UINT startIndex;
				UINT indexCount;
			};

			struct ObjectContainer
			{
				std::vector<Mesh> meshes;
				Material *mat;
			};

			RenderableObject( _In_ ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &materialContainers,
								D3D11_PRIMITIVE_TOPOLOGY meshTopology );
			~RenderableObject();

			DirectX::XMFLOAT4X4 getModelMatrix() const;
			void draw( _In_ Camera *renderCam );

		private:
			// no copying allowed
			RenderableObject( const RenderableObject& );
			RenderableObject& operator=( const RenderableObject& );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			MeshBuffer m_meshBuff;
			std::vector<ObjectContainer> m_matContainers;

			//TODO: voorlopig heeft deze even een modelMatrix, maar later een aparte parentclass maken voor een movableObject oid
			DirectX::XMFLOAT4X4 m_modelMatrix;

			D3D11_PRIMITIVE_TOPOLOGY m_topology;
	};
}
