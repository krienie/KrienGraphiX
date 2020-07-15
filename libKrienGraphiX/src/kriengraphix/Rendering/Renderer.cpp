
#include "Rendering/Renderer.h"

#include "Core/KGXCore.h"
#include "Core/RenderCollection.h"
#include "Core/RenderThread.h"
#include "Core/SceneView.h"
#include "Rendering/Passes/GBufferPass.h"
#include "Rendering/Passes/CompositionPass.h"
#include "Simulation/Scene.h"

namespace kgx
{
    void Renderer::render(Scene* scene, SceneView* view)
    {
        RenderCollection culler;
        culler.cullView(scene);

        RenderThread *renderThread = KGXCore::get()->getRenderThread();

        renderThread->enqueueImmediateCommand([view](ID3D11DeviceContext*)
        {
            view->clear();
        });

        GBufferPass gbuffPass(view);
        renderThread->enqueueImmediateCommand([&gbuffPass, &culler, scene](ID3D11DeviceContext*)
        {
            gbuffPass.record(&culler, *scene->getShaderLibrary(), *scene->getSceneTextures());
        });

        CompositionPass compositionPass(view);
        renderThread->enqueueImmediateCommand([&compositionPass, scene](ID3D11DeviceContext*)
        {
            compositionPass.record(scene->getLightData(), *scene->getShaderLibrary(), *scene->getSceneTextures());
        });

        gbuffPass.submit();
        compositionPass.submit();

        renderThread->enqueueImmediateCommand([view](ID3D11DeviceContext*)
        {
            view->present();
        });

        renderThread->flush();
        renderThread->incrementFrame();
    }
}
