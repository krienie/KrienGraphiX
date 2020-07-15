
#pragma once

namespace kgx
{
    class Scene;
    class SceneView;

    class Renderer
    {
        public:
            Renderer() = default;
            ~Renderer() = default;

            void render(Scene *scene, SceneView *view);
    };
}