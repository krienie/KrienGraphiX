
#include "Simulation/Scene.h"

#include <iostream>
#include <d3d11.h>

#include "Core/KGXCore.h"
#include "Core/RenderWindow.h"
#include "Rendering/Passes/CompositionPass.h"
#include "Rendering/Passes/GBufferPass.h"
#include "Simulation/SceneObject.h"

namespace kgx
{
    Scene::Scene()
        : m_lightData(), m_sceneTextures(), m_shaderProgLibrary(true), m_sceneObjects()
    {
        m_lightData.ambientLight = DirectX::XMFLOAT4( 0.25f, 0.25f, 0.25f, 1.0f );
    }

    Scene::~Scene()
    {
        for ( auto &obj : m_sceneObjects )
            delete obj;
    }
   
    DirectX::XMFLOAT3 Scene::getAmbient() const
    {
        return DirectX::XMFLOAT3( m_lightData.ambientLight.x, m_lightData.ambientLight.y, m_lightData.ambientLight.z );
    }

    void Scene::setAmbient( const DirectX::XMFLOAT3 &color )
    {
        m_lightData.ambientLight = DirectX::XMFLOAT4( color.x, color.y, color.z, 1.0f );
    }
    void Scene::setAmbient( float r, float g, float b )
    {
        m_lightData.ambientLight = DirectX::XMFLOAT4( r, g, b, 1.0f );
    }

    void Scene::addDirectionalLight( const DirectX::XMFLOAT3 &direction, float intensity )
    {
        // push a Light struct in the vector
        m_lightData.lights.push_back( { direction, intensity } );
    }

    void Scene::addSceneObject(SceneObject *obj)
    {
        // Load all textures in the material
        /*Material mat = obj->getMaterial();
        if ( !mat.diffuseMap.empty() )
            m_sceneTextures.loadFromDisk( mat.diffuseMap );
        if ( !mat.normalMap.empty() )
            m_sceneTextures.loadFromDisk( mat.normalMap );
        if ( !mat.specularMap.empty() )
            m_sceneTextures.loadFromDisk( mat.specularMap );*/

        m_sceneObjects.push_back( obj );
    }

    const std::vector<SceneObject *>* Scene::getSceneObjects() const
    {
        return &m_sceneObjects;
    }

    LightData Scene::getLightData() const
    {
        return m_lightData;
    }

    TextureLibrary* Scene::getSceneTextures()
    {
        return &m_sceneTextures;
    }

    ShaderProgramLibrary* Scene::getShaderLibrary()
    {
        return &m_shaderProgLibrary;
    }

    /*void Scene::render( Camera *renderCam, const D3D11_VIEWPORT &vp, ID3D11RasterizerState *rs,
                        ID3D11RenderTargetView *rtv, ID3D11DepthStencilView *dsv )
    {
        if ( !m_preparedForRender )
        {
            //TODO: change this so each worker thread has its own deferred context
            //		prob move this deferredContext creation to thread implementation
            // create deferred context
            ID3D11Device *dxDev = KGXCore::get()->getDxDevicePtr();
            dxDev->CreateDeferredContext( 0, &m_dxDeferredDevCont );

            RenderWindow *renderWindow = KGXCore::get()->getRenderWindow();
            m_gbufferPass = new GBufferPass( m_dxDeferredDevCont, dsv, renderCam, renderWindow->getWidth(), renderWindow->getHeight() );

            m_compositionPass = new CompositionPass( m_dxDeferredDevCont, dsv, rtv );

            m_preparedForRender = true;
        }

        m_dxDeferredDevCont->RSSetViewports( 1, &vp );
        m_dxDeferredDevCont->RSSetState( rs );
        m_gbufferPass->record( m_sceneObjects, m_lightData, m_sceneTextures );

        m_dxDeferredDevCont->RSSetViewports( 1, &vp );
        m_dxDeferredDevCont->RSSetState( rs );
        m_compositionPass->record( m_sceneObjects, m_lightData, m_gbufferPass->getTextureLibrary() );

        // execute render passes
        m_gbufferPass->submit();
        m_compositionPass->submit();
    }*/
}
