
#include "Simulation/MeshComponent.h"
#include "Rendering/MeshBuffer.h"

namespace kgx
{
    MeshComponent::MeshComponent(SceneObject *owner, D3D11_PRIMITIVE_TOPOLOGY topology, MeshBuffer* meshBuffer,
                                    UINT indexCount, UINT startIndex, UINT baseVertex)
        : SceneObjectComponent(owner), m_topology(topology), m_meshBuffer(meshBuffer), m_indexCount(indexCount),
            m_startIndex(startIndex), m_baseVertex(baseVertex), m_material()
    {
    }

    MeshComponent::~MeshComponent()
    {
        delete m_meshBuffer;
    }

    void MeshComponent::setMaterial(const Material &material)
    {
        m_material = material;
    }

    Material MeshComponent::getMaterial() const
    {
        return m_material;
    }

    D3D11_PRIMITIVE_TOPOLOGY MeshComponent::getTopology() const
    {
        return m_topology;
    }

    const MeshBuffer* MeshComponent::getMeshBuffer() const
    {
        return m_meshBuffer;
    }

    UINT MeshComponent::getIndexCount() const
    {
        return m_indexCount;
    }

    UINT MeshComponent::getStartIndex() const
    {
        return m_startIndex;
    }

    UINT MeshComponent::getBaseVertex() const
    {
        return m_baseVertex;
    }
}
