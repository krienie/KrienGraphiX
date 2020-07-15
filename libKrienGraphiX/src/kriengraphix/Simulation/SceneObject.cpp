
#include "Simulation/SceneObject.h"

#include "Simulation/SceneObjectComponent.h"

namespace kgx
{
    SceneObject::SceneObject(std::string name)
        : m_isDirty(true), m_name(std::move(name)), m_xPos(0.0f), m_yPos(0.0f), m_zPos(0.0f),
        m_xScale(1.0f), m_yScale(1.0f), m_zScale(1.0f),
        m_modelMatrix(), m_normalMatrix(), m_sceneComponents()
    {
    }

    void SceneObject:: setPosition(float xPos, float yPos, float zPos)
    {
        KGXCore::get()->getSceneThread()->enqueueSingleCommand([this, xPos, yPos, zPos](Scene*)
        {
            m_xPos = xPos;
            m_yPos = yPos;
            m_zPos = zPos;

            m_isDirty = true;
        });
    }

    void SceneObject::setScale(float xScale, float yScale, float zScale)
    {
        KGXCore::get()->getSceneThread()->enqueueSingleCommand([this, xScale, yScale, zScale](Scene*)
        {
            m_xScale = xScale;
            m_yScale = yScale;
            m_zScale = zScale;

            m_isDirty = true;
        });
    }

    std::string SceneObject::getName() const
    {
        return m_name;
    }

    DirectX::XMFLOAT4X4 SceneObject::getModelMatrix()
    {
        if ( m_isDirty )
        {
            const DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation( m_xPos, m_yPos, m_zPos );
            const DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling( m_xScale, m_yScale, m_zScale );
            const DirectX::XMMATRIX modelMat = DirectX::XMMatrixMultiply( scaleMat, transMat );

            DirectX::XMStoreFloat4x4( &m_modelMatrix, modelMat );

            m_isDirty = false;
        }

        return m_modelMatrix;
    }

    DirectX::XMFLOAT4X4 SceneObject::getNormalMatrix()
    {
        if ( m_isDirty )
        {
            const DirectX::XMMATRIX modelMat = DirectX::XMLoadFloat4x4( &m_modelMatrix );
            DirectX::XMMATRIX normalMat = DirectX::XMMatrixInverse( nullptr, modelMat );
            normalMat = DirectX::XMMatrixTranspose( normalMat );

            DirectX::XMStoreFloat4x4( &m_normalMatrix, normalMat );

            m_isDirty = false;
        }

        return m_normalMatrix;
    }

    std::vector<std::shared_ptr<SceneObjectComponent>> SceneObject::getComponents() const
    {
        //TODO(KL): build a check that this method is called from a SceneThread command
        return m_sceneComponents;
    }
}
