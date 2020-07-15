
#pragma once

#include <unordered_map>
#include <vector>

#include <DirectXMath.h>

#include "Simulation/Defines.h"
#include "Rendering/TextureLibrary.h"
#include "Rendering/ShaderProgramLibrary.h"

namespace kgx
{
    class SceneObject;
    class ShaderProgram;
    class CompositionPass;
    class GBufferPass;

    class Scene
    {
        public:
            Scene();
            ~Scene();

            Scene(const Scene&) noexcept = delete;
            Scene& operator=(const Scene&) noexcept = delete;
            Scene(Scene&&) noexcept = delete;
            Scene& operator=(Scene&&) noexcept = delete;

            DirectX::XMFLOAT3 getAmbient() const;
            void setAmbient( const DirectX::XMFLOAT3 &color );
            void setAmbient( float r, float g, float b );
            void addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity );

            void addSceneObject(SceneObject *obj);
            [[nodiscard]]
            const std::vector<SceneObject*>* getSceneObjects() const;

            //TODO(KL): This can probably stored in a cleaner way
            LightData getLightData() const;
            TextureLibrary* getSceneTextures();
            ShaderProgramLibrary* getShaderLibrary();

            //void render( Camera *renderCam, const D3D11_VIEWPORT &vp, ID3D11RasterizerState *rs,
            //             ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv );

        private:
            LightData m_lightData;
            TextureLibrary m_sceneTextures;
            ShaderProgramLibrary m_shaderProgLibrary;
            std::vector<SceneObject*> m_sceneObjects;
    };
}
