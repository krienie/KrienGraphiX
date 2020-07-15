
#pragma once

namespace kgx
{
    class SceneObject;

    class SceneObjectComponent
    {
        public:
            SceneObjectComponent(SceneObject *owner);
            virtual ~SceneObjectComponent() = default;

            [[nodiscard]]
            SceneObject* getOwner() const;

        private:
            SceneObject *m_owner;
    };
}
