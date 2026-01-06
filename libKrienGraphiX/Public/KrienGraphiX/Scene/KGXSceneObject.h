
#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <mutex>

#include "Private/Math/MathDefines.h"
#include "Private/Math/Transform.h"

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
	explicit KGXSceneObject(const std::string& name);
	virtual ~KGXSceneObject() = default;

	core::KGXScene* getParentScene() const;
	void setPosition(float xPos, float yPos, float zPos);
	void setRotation(float pitch, float yaw, float roll);
	void setScale(float xScale, float yScale, float zScale);

	bool hasTransformChangedThisFrame() const { return mHasTransformChanged; }

	[[nodiscard]]
	std::string getName() const;

	[[nodiscard]]
	math::Matrix4X4 getWorldTransform() const;

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

private:
	virtual void updateImpl([[maybe_unused]] float deltaTime) {}

	void addNewComponentInternal(KGXSceneObjectComponent* newComponent);

	bool mHasTransformChanged = true;

	core::KGXScene* mParentScene = nullptr;

	std::string mName;

	math::Transform mTransform;

	std::vector<std::shared_ptr<KGXSceneObjectComponent>> mSceneComponents;
};

class KGXBoxObject : public KGXSceneObject
{
public:
	KGXBoxObject(const std::string& name);
};

class KGXCameraObject : public KGXSceneObject
{
public:
	KGXCameraObject(const std::string& name);
};
}
