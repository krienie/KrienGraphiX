
#pragma once

#include <map>

#include "../parsers/KGParserDefines.h"


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
			KGMaterialLibraryGenerator( const KGMaterialLibraryGenerator& );
			KGMaterialLibraryGenerator& operator=(const KGMaterialLibraryGenerator&);

			static std::string float4ToString( const DirectX::XMFLOAT4 &float4 );
	};
}
