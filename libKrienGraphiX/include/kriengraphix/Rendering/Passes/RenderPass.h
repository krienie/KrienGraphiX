
#pragma once

#include "Core/RenderThread.h"

namespace kgx
{
    class DeferredRenderCommandList;
    class SceneView;

    class RenderPass
    {
        public:
            explicit RenderPass(SceneView* view);
            virtual ~RenderPass() = default;

            void submit() const;

        protected:
            [[nodiscard]]
            SceneView* getSceneView() const;
            DeferredRenderCommandList* getCommandList();
            void finishCommandList();

        private:
            SceneView *m_sceneView;
            DeferredRenderCommandList m_commandList;
    };
}
