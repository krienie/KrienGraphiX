
#pragma once

#include "Core/Commands/CommandBase.h"

namespace kgx
{
    class Scene;

    template<class Lambda>
    class SceneCommand final : public CommandBase
    {
        public:
            SceneCommand(Scene *scene, Lambda func)
                : CommandBase(), m_scene(scene), m_commandFunc(std::forward<Lambda>(func))
            {
            }

            virtual ~SceneCommand() = default;
            void execute() override
            {
                m_commandFunc(m_scene);
            }

        private:
            Scene *m_scene;
            Lambda m_commandFunc;
    };
}
