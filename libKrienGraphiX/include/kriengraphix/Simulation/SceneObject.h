
#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <DirectXMath.h>

#include "Core/KGXCore.h"
#include "Core/SceneThread.h"

namespace kgx
{
    class SceneObjectComponent;

    class SceneObject
    {
        public:
            explicit SceneObject(std::string name);
            ~SceneObject() = default;

            void setPosition(float xPos, float yPos, float zPos);
            void setScale(float xScale, float yScale, float zScale);

            [[nodiscard]]
            std::string getName() const;

            [[nodiscard]]
            DirectX::XMFLOAT4X4 getModelMatrix();
            [[nodiscard]]
            DirectX::XMFLOAT4X4 getNormalMatrix();

            template<class Comp,
                        std::enable_if_t<std::is_base_of<SceneObjectComponent, Comp>::value, int> = 0,
                        typename... Args>
            Comp* addNewComponent(Args... args)
            {
                Comp *newObj = new Comp(this, args...);
                KGXCore::get()->getSceneThread()->enqueueSingleCommand([this, newObj](Scene*) { m_sceneComponents.emplace_back(newObj); });

                return newObj;
            }

            [[nodiscard]]
            std::vector<std::shared_ptr<SceneObjectComponent>> getComponents() const;

        private:
            bool m_isDirty;

            std::string m_name;

            float m_xPos;
            float m_yPos;
            float m_zPos;
            float m_xScale;
            float m_yScale;
            float m_zScale;

            DirectX::XMFLOAT4X4 m_modelMatrix;
            DirectX::XMFLOAT4X4 m_normalMatrix;

            std::vector<std::shared_ptr<SceneObjectComponent>> m_sceneComponents;
    };
}
