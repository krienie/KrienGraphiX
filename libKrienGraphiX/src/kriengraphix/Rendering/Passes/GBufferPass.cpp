
#include "Rendering/Passes/GBufferPass.h"

#include <memory>

#include "Core/PhysXManager.h"
#include "Core/RenderThread.h"
#include "Core/SceneView.h"
#include "Simulation/CameraComponent.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/MeshBuffer.h"
#include "Rendering/RenderTexture.h"
#include "Rendering/ShaderProgram.h"
#include "Rendering/ShaderProgramLibrary.h"
#include "Rendering/Shaders/VertexShader.h"
#include "Rendering/Shaders/PixelShader.h"
#include "Rendering/TextureLibrary.h"

namespace kgx
{
    GBufferPass::GBufferPass(SceneView *view)
        : RenderPass(view)
    {
    }

    void GBufferPass::record(const RenderCollection *renderCollection, ShaderProgramLibrary &shaderLibrary, TextureLibrary &sceneTextures)
    {
        SceneView *view = getSceneView();
        if (!view)
        {
            // Nothing to do if we don't have a valid SceneView
            return;
        }

        std::shared_ptr<CameraComponent> camera = renderCollection->getCamera();
        if (!camera)
        {
            // Also nothing to do if we don't have a valid CameraComponent
            return;
        }


        DeferredRenderCommandList *commandList = getCommandList();
        const std::shared_ptr<RenderTexture> diffuseTex = sceneTextures.getRenderTexture(static_cast<UINT>(view->getWidth()), static_cast<UINT>(view->getHeight()));
        const std::shared_ptr<RenderTexture> depthTex   = sceneTextures.getRenderTexture(static_cast<UINT>(view->getWidth()), static_cast<UINT>(view->getHeight()));

        // Diffuse render texture
        ID3D11RenderTargetView *diffuseBuffView = diffuseTex->getRenderTargetView();
        commandList->record([diffuseBuffView](ID3D11DeviceContext *dxDeferredDevCont)
        {
            float diffuseClearColor[4] = {0.4f, 0.4f, 0.4f, 1.0f};
            dxDeferredDevCont->ClearRenderTargetView(diffuseBuffView, diffuseClearColor);
        });

        // Depth render texture
        ID3D11RenderTargetView *depthBuffView = depthTex->getRenderTargetView();
        commandList->record([depthBuffView](ID3D11DeviceContext *dxDeferredDevCont)
        {
            float depthClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            dxDeferredDevCont->ClearRenderTargetView(depthBuffView, depthClearColor);
        });

        // Set output render target views
        ID3D11RenderTargetView *rtvOutput[2] = {diffuseBuffView, depthBuffView};
        commandList->record([rtvOutput, view](ID3D11DeviceContext *dxDeferredDevCont)
        {
            dxDeferredDevCont->OMSetRenderTargets(2, rtvOutput, view->getDepthStencil());
        });

        // bind shaders to pipeline
        ShaderProgram *gbuffProg = shaderLibrary.getShaderProgram("DeferredGBuffer");
        commandList->record([gbuffProg](ID3D11DeviceContext *dxDeferredDevCont)
        {
            gbuffProg->activate(dxDeferredDevCont);
        });

        // set per camera data
        ConstantBuffer *vertexPerCamBuffer = gbuffProg->getVertexShader()->getConstantBufferPtrByIndex(0);
        DirectX::XMFLOAT4X4 perCamData[2] = {camera->getViewMatrix(), camera->getProjMatrix()};
        commandList->record([vertexPerCamBuffer, perCamData](ID3D11DeviceContext *dxDeferredDevCont)
        {
            vertexPerCamBuffer->copyBufferData(perCamData, sizeof(DirectX::XMFLOAT4X4) * 2u, dxDeferredDevCont);
        });

        //for ( it = renderObjects.begin(); it != renderObjects.end(); ++it )
        for (const auto &culledObj : *renderCollection->getCulledObjects())
        {
            // get model/normal matrix
            DirectX::XMMATRIX identityMat = DirectX::XMMatrixIdentity();
            DirectX::XMFLOAT4X4 modelMatrix;
            DirectX::XMStoreFloat4x4(&modelMatrix, identityMat);
            if (!PhysXManager::getInst()->getShapeGlobalPosition(culledObj.name, modelMatrix))
            {
                modelMatrix = culledObj.modelMatrix;
            }

            DirectX::XMFLOAT4X4 matrices[2] = {modelMatrix, culledObj.normalMatrix};

            // set model/normal matrix
            ConstantBuffer *vertexPerObjBuffer = gbuffProg->getVertexShader()->getConstantBufferPtrByIndex(1);
            commandList->record([vertexPerObjBuffer, matrices](ID3D11DeviceContext *dxDeferredDevCont)
            {
                vertexPerObjBuffer->copyBufferData(matrices, sizeof(DirectX::XMFLOAT4X4) * 2u, dxDeferredDevCont);
            });

            for (const auto &meshComp : culledObj.meshComponents)
            {
                // get material constants and shader resources (textures and such)
                Material mat = meshComp->getMaterial();
                int useTextures = static_cast<int>(!mat.diffuseMap.empty());
                DirectX::XMFLOAT4 useTextures4;
                useTextures4.x = static_cast<float>(useTextures);

                DirectX::XMFLOAT4 materialData[3] = {mat.diffuse, mat.specular, useTextures4};

                // get material constants and shader resources (textures and such)
                ConstantBuffer *pixelPerObjectData = gbuffProg->getPixelShader()->getConstantBufferPtrByIndex(0);
                commandList->record([pixelPerObjectData, materialData](ID3D11DeviceContext *dxDeferredDevCont)
                {
                    pixelPerObjectData->copyBufferData(materialData, sizeof(DirectX::XMFLOAT4) * 3u, dxDeferredDevCont);
                });

                // set default texture sampler
                ID3D11SamplerState *samplers[1] = {sceneTextures.getDefaultSampler()};
                commandList->record([samplers](ID3D11DeviceContext *dxDeferredDevCont)
                {
                    dxDeferredDevCont->PSSetSamplers(0u, 1u, samplers);
                });

                // get textures
                std::vector<std::string> textures;
                if (!mat.diffuseMap.empty())
                    textures.push_back(mat.diffuseMap);
                if (!mat.specularMap.empty())
                    textures.push_back(mat.specularMap);
                if (!mat.normalMap.empty())
                    textures.push_back(mat.normalMap);

                // collect all ShaderResourceViews for all used textures
                std::vector<ID3D11ShaderResourceView *> texViews;
                texViews.reserve(textures.size());

                std::vector<std::string>::const_iterator texIt;
                for (texIt = textures.cbegin(); texIt != textures.cend(); ++texIt)
                {
                    Texture *texture = sceneTextures.getTexture(*texIt);
                    texViews.push_back(texture->getResourceView());       // assumes the texture is always found..
                }

                // set texture ShaderResourceViews for the pixel shader
                commandList->record([texViews](ID3D11DeviceContext *dxDeferredDevCont)
                {
                    dxDeferredDevCont->PSSetShaderResources(0u, static_cast<UINT>(texViews.size()), texViews.data());
                });

                commandList->record([meshComp](ID3D11DeviceContext *dxDeferredDevCont)
                {
                    // bind Vertex- and Index-buffers to IA Stage
                    const MeshBuffer *meshBuff = meshComp->getMeshBuffer();
                    UINT bufferStride = meshBuff->getVertexBufferStride();
                    UINT offset = 0;

                    ID3D11Buffer *vertexBuffer = meshBuff->getVertexBuffer();
                    dxDeferredDevCont->IASetVertexBuffers(0, 1, &vertexBuffer, &bufferStride, &offset);
                    dxDeferredDevCont->IASetIndexBuffer(meshBuff->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
                    dxDeferredDevCont->IASetPrimitiveTopology(meshComp->getTopology());

                    // draw indexed
                    dxDeferredDevCont->DrawIndexed(meshComp->getIndexCount(), meshComp->getStartIndex(), meshComp->getBaseVertex());
                });
            }
        }

        finishCommandList();
    }
}
