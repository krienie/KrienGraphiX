
#pragma once

#include "Core/RenderCollection.h"
#include "Rendering/Passes/RenderPass.h"

namespace kgx
{
    struct LightData;
    class CameraComponent;
    class DeferredRenderCommandList;
    class RenderTexture;
    class SceneObject;
    class ShaderProgramLibrary;
    class TextureLibrary;

    class GBufferPass final : public RenderPass
    {
        public:
            GBufferPass(SceneView* view);
            virtual ~GBufferPass() = default;

            void record(const RenderCollection *renderCollection, ShaderProgramLibrary &shaderLibrary, TextureLibrary &sceneTextures);
    };
}
