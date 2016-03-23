
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

			RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<Mesh> &meshContainers,
								D3D11_PRIMITIVE_TOPOLOGY meshTopology );
			RenderableObject( ID3D11Device *dxDevice, MeshBuffer buff, const std::vector<Mesh> &meshContainers,
							  D3D11_PRIMITIVE_TOPOLOGY meshTopology, const std::string &name );
			~RenderableObject();

			std::string getOriginalFilename() const;
			void setOriginalFilename( const std::string &filename );

			void draw( Camera *renderCam, ShaderProgram *shaderProg );

		private:
			friend class KGObjectGenerator;

			// no copying allowed
			RenderableObject( const RenderableObject& );
			RenderableObject& operator=( const RenderableObject& );

			ID3D11Device *m_dxDev;
			ID3D11DeviceContext *m_dxDevCont;

			MeshBuffer m_meshBuff;
			std::vector<Mesh> m_meshContainers;

			D3D11_PRIMITIVE_TOPOLOGY m_topology;

			std::string m_originalFilename;
	};
}
