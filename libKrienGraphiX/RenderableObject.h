
#pragma once

#include <d3d11.h>
#include <vector>

#include "Defines.h"
#include "MovableObject.h"


namespace kgx
{
	class Camera;
	class ShaderProgram;

	class RenderableObject : public MovableObject
	{
		public:
			struct Mesh
			{
				Mesh( std::string nm = "", UINT sIdx = 0U, UINT idxCnt = 0U ) : name(nm), startIndex(sIdx), indexCount(idxCnt) {}

				std::string name;
				UINT startIndex;
				UINT indexCount;

				inline bool operator< (const Mesh& rhs) const { return startIndex < rhs.startIndex; }
				inline bool operator==(const Mesh& rhs) const { return startIndex == rhs.startIndex; }
			};

			struct ObjectContainer
			{
				ObjectContainer( const std::vector<Mesh> &me, ShaderProgram *sp ) : meshes(me), shaderProg(sp) {}

				std::vector<Mesh> meshes;
				ShaderProgram *shaderProg;
			};

			RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &objectContainers,
								D3D11_PRIMITIVE_TOPOLOGY meshTopology );
			RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<ObjectContainer> &objectContainers,
							  D3D11_PRIMITIVE_TOPOLOGY meshTopology, const std::string &name );
			~RenderableObject();

			void draw( Camera *renderCam, const std::vector<Light> &lights, const DirectX::XMFLOAT4 &ambientColor );

		private:
			friend class KGObjectGenerator;

			// no copying allowed
			RenderableObject( const RenderableObject& );
			RenderableObject& operator=( const RenderableObject& );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			MeshBuffer m_meshBuff;
			std::vector<ObjectContainer> m_objContainers;

			D3D11_PRIMITIVE_TOPOLOGY m_topology;
	};
}
