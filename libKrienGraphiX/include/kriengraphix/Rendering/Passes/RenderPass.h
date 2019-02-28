
#pragma once

#include <vector>

#include "Simulation/RenderableObject.h"

struct ID3D11DeviceContext;
struct ID3D11CommandList;

namespace kgx
{
    class Camera;
    class TextureLibrary;

    class RenderPass
    {
        public:
            explicit RenderPass( ID3D11DeviceContext *deferredDevCont );
            virtual ~RenderPass();

            virtual void record( const std::vector<RenderableObject*> &renderObjects, const LightData &lightData,
                                 const TextureLibrary &sceneTextures ) = 0;
            void submit() const;

        protected:
            ID3D11DeviceContext* getDeferredContext() const;
            void finishCommandList();

        private:
            ID3D11DeviceContext *m_dxDevCont;
            ID3D11DeviceContext *m_dxDeferredDevCont;
            ID3D11CommandList *m_commandList;
    };
}
