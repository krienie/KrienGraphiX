
#pragma once

#include "Rendering/Passes/RenderPass.h"

namespace kgx
{
    struct LightData;
    class TextureLibrary;
    class ShaderProgramLibrary;

    class CompositionPass final : public RenderPass
    {
        public:
            CompositionPass(SceneView *view);
            virtual ~CompositionPass() = default;

            void record(const LightData &lightData, ShaderProgramLibrary &shaderLibrary, TextureLibrary &sceneTextures);
    };
}
