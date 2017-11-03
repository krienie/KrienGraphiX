
#pragma once

#include <unordered_map>

#include "../parsers/KGParserDefines.h"

namespace kgx
{
	class KGMaterialLibraryParser
	{
		public:
			static bool parse( const std::string &input, std::unordered_map<std::string, KgMatData> &outputData );

			// no copying allowed
			KGMaterialLibraryParser( const KGMaterialLibraryParser& )            = delete;
			KGMaterialLibraryParser& operator=( const KGMaterialLibraryParser& ) = delete;

		private:
			KGMaterialLibraryParser();
			//~KGMaterialLibraryParser();
	};
}
