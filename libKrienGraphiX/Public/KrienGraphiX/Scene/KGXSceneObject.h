
#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// TODO(KL): We're temporarily using DirectXMath directly until there is a more generic replacement for it
#include <DirectXMath.h>
#include <mutex>

namespace kgx::core
{
class KGXScene;
}

namespace kgx
{
class KGXSceneObjectComponent;

class KGXSceneObject
{
public:
    explicit KGXSceneObject(std::string name);
    virtual ~KGXSceneObject() = default;

    void setParentScene(core::KGXScene& parentScene);
    void setPosition(float xPos, float yPos, float zPos);
    void setRotation(float pitch, float yaw, float roll);
    void setScale(float xScale, float yScale, float zScale);

    [[nodiscard]]
    std::string getName() const;

    [[nodiscard]]
    DirectX::XMFLOAT4X4 getModelMatrix() const;
    [[nodiscard]]
    DirectX::XMFLOAT4X4 getNormalMatrix() const;

    template<class Comp,
                std::enable_if_t<std::is_base_of_v<KGXSceneObjectComponent, Comp>, int> = 0,
                typename... Args>
    Comp* addNewComponent(Args... args)
    {
        Comp *newObj = new Comp(this, args...);
        addNewComponentInternal(newObj);

        return newObj;
    }

    void update(float deltaTime);

    [[nodiscard]]
    std::vector<std::shared_ptr<KGXSceneObjectComponent>> getComponents() const;

protected:
    virtual void updateInternal([[maybe_unused]] float deltaTime) {}

private:
    void addNewComponentInternal(KGXSceneObjectComponent* newComponent);

    bool mIsDirty;

    core::KGXScene* mParentScene = nullptr;

    std::string mName;

    //TODO(KL): Place all this in a Transform class
    float mXPos = 0;
    float mYPos = 0;
    float mZPos = 0;
    float mPitch = 0;
    float mYaw = 0;
    float mRoll = 0;
    float mXScale = 1;
    float mYScale = 1;
    float mZScale = 1;

    DirectX::XMFLOAT4X4 mModelMatrix;
    DirectX::XMFLOAT4X4 mNormalMatrix;

    std::vector<std::shared_ptr<KGXSceneObjectComponent>> mSceneComponents;

    mutable std::mutex mUpdateMutex;
};
}
