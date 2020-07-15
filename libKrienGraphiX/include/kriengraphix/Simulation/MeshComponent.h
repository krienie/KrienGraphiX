
#pragma once

#include <d3d11.h>

#include "Simulation/Defines.h"
#include "Simulation/SceneObjectComponent.h"

namespace kgx
{
    class MeshBuffer;

    class MeshComponent final : public SceneObjectComponent
    {
        public:
            MeshComponent(SceneObject *owner, D3D11_PRIMITIVE_TOPOLOGY topology, MeshBuffer* meshBuffer,
                            UINT indexCount, UINT startIndex, UINT baseVertex);
            virtual ~MeshComponent();
            
            void setMaterial(const Material& material);
            Material getMaterial() const;

            D3D11_PRIMITIVE_TOPOLOGY getTopology() const;
            const MeshBuffer* getMeshBuffer() const;
            UINT getIndexCount() const;
            UINT getStartIndex() const;
            UINT getBaseVertex() const;

        private:
            D3D11_PRIMITIVE_TOPOLOGY m_topology;
            MeshBuffer* m_meshBuffer;

            UINT m_indexCount;
            UINT m_startIndex;
            UINT m_baseVertex;

            Material m_material;
    };
}
