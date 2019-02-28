
#pragma once

#include "IO/KGParserDefines.h"

namespace kgx
{
    class KGMaterialLibraryGenerator
    {
        public:
            //TODO: maybe make these functions non-static..
            static void generate( const std::vector<KgMatData> &inputData, std::string &output );
            static void append( const std::vector<KgMatData> &inputData, std::string &matLibString );

        private:
            KGMaterialLibraryGenerator();
            //~KGMaterialLibraryGenerator();

            // no copying allowed
            KGMaterialLibraryGenerator( const KGMaterialLibraryGenerator& ) = delete;
            KGMaterialLibraryGenerator& operator=( const KGMaterialLibraryGenerator& ) = delete;

            static std::string float4ToString( const DirectX::XMFLOAT4 &float4 );
    };
}
