
#include "KrienGraphiX/Scene/KGXSceneObject.h"

#include "KGXScene.h"
#include "KrienGraphiX/Scene/KGXSceneObjectComponent.h"

namespace kgx
{
KGXSceneObject::KGXSceneObject(std::string name)
    : mIsDirty(true), mName(std::move(name)),
    mModelMatrix(), mNormalMatrix()
{
}

void KGXSceneObject::setParentScene(core::KGXScene& parentScene)
{
    mParentScene = &parentScene;
}

void KGXSceneObject:: setPosition(float xPos, float yPos, float zPos)
{
    std::lock_guard lock(mUpdateMutex);

    mXPos = xPos;
    mYPos = yPos;
    mZPos = zPos;

    mIsDirty = true;
}

void KGXSceneObject::setRotation(float pitch, float yaw, float roll)
{
    std::lock_guard lock(mUpdateMutex);

    mPitch = pitch;
    mYaw = yaw;
    mRoll = roll;

    mIsDirty = false;
}

void KGXSceneObject::setScale(float xScale, float yScale, float zScale)
{
    std::lock_guard lock(mUpdateMutex);

    mXScale = xScale;
    mYScale = yScale;
    mZScale = zScale;

    mIsDirty = true;
}

std::string KGXSceneObject::getName() const
{
    return mName;
}

DirectX::XMFLOAT4X4 KGXSceneObject::getModelMatrix() const
{
    std::lock_guard lock(mUpdateMutex);
    return mModelMatrix;
}

DirectX::XMFLOAT4X4 KGXSceneObject::getNormalMatrix() const
{
    std::lock_guard lock(mUpdateMutex);
    return mNormalMatrix;
}

void KGXSceneObject::update(float deltaTime)
{
    {
        std::lock_guard lock(mUpdateMutex);

        if ( mIsDirty )
        {
            // Model matrix
            const DirectX::XMMATRIX rotMat   = DirectX::XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll);
            const DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(mXPos, mYPos, mZPos);
            const DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(mXScale, mYScale, mZScale);
            const DirectX::XMMATRIX modelMat = DirectX::XMMatrixMultiply(rotMat, DirectX::XMMatrixMultiply(scaleMat, transMat));
            DirectX::XMStoreFloat4x4( &mModelMatrix, modelMat );

            // Normal matrix
            DirectX::XMMATRIX normalMat = DirectX::XMMatrixInverse( nullptr, modelMat );
            normalMat = DirectX::XMMatrixTranspose( normalMat );
            DirectX::XMStoreFloat4x4( &mNormalMatrix, normalMat );

            mIsDirty = false;
        }
    }

    updateInternal(deltaTime);
}

std::vector<std::shared_ptr<KGXSceneObjectComponent>> KGXSceneObject::getComponents() const
{
    std::lock_guard lock(mUpdateMutex);

    return mSceneComponents;
}

void KGXSceneObject::addNewComponentInternal(KGXSceneObjectComponent* newComponent)
{
    std::lock_guard lock(mUpdateMutex);

    newComponent->initialize();

    mSceneComponents.emplace_back(newComponent);
}
}
