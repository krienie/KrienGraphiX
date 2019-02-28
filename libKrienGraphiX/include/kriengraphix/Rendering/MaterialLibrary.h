
#pragma once

#include <unordered_map>

#include "Simulation/Defines.h"

namespace kgx
{
    class MaterialLibrary
    {
        public:
            MaterialLibrary();
            ~MaterialLibrary() = default;

            Material getMaterial( const std::string &matName );

        private:
            bool loadMaterials();

            std::unordered_map<std::string, Material> m_materials;
    };
}
