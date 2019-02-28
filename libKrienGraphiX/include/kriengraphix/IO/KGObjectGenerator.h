
#pragma once

#include "IO/KGParserDefines.h"

namespace kgx
{
    class KGObjectGenerator
    {
        public:
            static void generate( KgoData &inputData, std::string &output );

        private:
            KGObjectGenerator();
            //~KGObjectGenerator();

            // no copying allowed
            KGObjectGenerator( const KGObjectGenerator& ) = delete;
            KGObjectGenerator& operator=( const KGObjectGenerator& ) = delete;
    };
}
