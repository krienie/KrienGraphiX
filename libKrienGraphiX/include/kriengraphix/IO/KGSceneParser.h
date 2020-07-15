
#pragma once

#include <string>

namespace kgx
{
    class SceneThread;

    class KGSceneParser
    {
        public:
            static void loadKGScene( const std::string &kgsceneFile );

            KGSceneParser() = delete;
            ~KGSceneParser() = delete;

        private:
            static bool loadSceneObject(const std::string &objString);
            static bool loadCamera(const std::string &camString);
            static bool loadLight(const std::string &lightString);
    };
}
