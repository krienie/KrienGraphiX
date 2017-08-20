
#pragma once

#include <string>
#include <unordered_map>
#include "KGParserDefines.h"

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
			KGShaderProgramsParser( const KGShaderProgramsParser& );
			KGShaderProgramsParser& operator=( const KGShaderProgramsParser& );

			static bool parseProgram( std::string &parseString, KgShaderProgData &progData );
	};
}
