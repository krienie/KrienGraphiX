
#pragma once

namespace kgx
{
class KGXSceneObject;

class KGXSceneObjectComponent
{
public:
    KGXSceneObjectComponent(KGXSceneObject* owner);
    virtual ~KGXSceneObjectComponent() = default;

    [[nodiscard]]
    KGXSceneObject* getOwner() const;

    virtual void initialize() { mIsInitialized = true; }
    virtual void update([[maybe_unused]] float deltaTime) {}

protected:
    bool mIsInitialized = false;

private:
    KGXSceneObject* mOwner;
};
}
