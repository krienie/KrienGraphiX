
#pragma once

#include <map>

#include "../parsers/KGParserDefines.h"


namespace kgx
{
	class KGMaterialLibraryParser
	{
		public:
			static bool parse( const std::string &input, std::map<std::string, KgMatData> &outputData );

		private:
			KGMaterialLibraryParser();
			//~KGMaterialLibraryParser();

			// no copying allowed
			KGMaterialLibraryParser( const KGMaterialLibraryParser& );
			KGMaterialLibraryParser& operator=(const KGMaterialLibraryParser&);
	};
}
