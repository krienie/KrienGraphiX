
#pragma once

#include <string>
#include <unordered_map>

#include "IO/KGParserDefines.h"

namespace kgx
{
    class KGShaderProgramsParser
    {
        public:
            static bool parse( const std::string &inputData, std::unordered_map<std::string, KgShaderProgData> &outputData );

        private:
            KGShaderProgramsParser();
            //~KGShaderProgramsParser();

            // no copying allowed
            KGShaderProgramsParser( const KGShaderProgramsParser& ) = delete;
            KGShaderProgramsParser& operator=( const KGShaderProgramsParser& ) = delete;

            static bool parseProgram( std::string &parseString, KgShaderProgData &progData );
    };
}
