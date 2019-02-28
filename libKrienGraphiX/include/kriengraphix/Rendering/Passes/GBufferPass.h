
#pragma once

#include "Rendering/Passes/RenderPass.h"
#include "Rendering/TextureLibrary.h"
#include "Rendering/ShaderProgramLibrary.h"

namespace kgx
{
    class Camera;
    class RenderTexture;
    class ShaderProgram;

    class GBufferPass : public RenderPass
    {
        public:
            GBufferPass( ID3D11DeviceContext *deferredDevCont, ID3D11DepthStencilView *dsv,
                         Camera *renderCamera, unsigned int width, unsigned int height );

            virtual ~GBufferPass();

            void record( const std::vector<RenderableObject*> &renderObjects, const LightData &lightData,
                         const TextureLibrary &sceneTextures ) override;

            const TextureLibrary& getTextureLibrary() const;

        private:
            TextureLibrary m_gbuffTextureLib;
            std::vector<RenderTexture*> m_gbuffTextures;

            ShaderProgram *m_shaderProg;
            ShaderProgramLibrary m_shaderProgLibrary;

            ID3D11DepthStencilView *m_depthStencilView;

            Camera *m_renderCam;
    };
}
