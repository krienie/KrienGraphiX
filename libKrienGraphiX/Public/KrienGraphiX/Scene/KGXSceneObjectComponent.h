
#pragma once

#include "KrienGraphiX/Math/MathDefines.h"
#include "KrienGraphiX/Math/Transform.h"

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

	void initialize();
	void update([[maybe_unused]] float deltaTime);

	void setPosition(const math::Vector3& position);
	void setRotation(const math::Quaternion& rotation);
	void setScale(const math::Vector3& scale);

	[[nodiscard]] math::Matrix4X4 getRelativeTransform() const;
	[[nodiscard]] math::Matrix4X4 getWorldTransform() const;

protected:
	bool hasTransformChangedThisFrame() const;

	bool mIsInitialized = false;

	math::Transform mTransform;

private:
	virtual bool initializeImpl() { return true; }
	virtual void updateImpl([[maybe_unused]] float deltaTime) {}

	KGXSceneObject* mOwner;

	bool mHasTransformChanged = true;
};
}
