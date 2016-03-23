
#pragma once

#include <string>

#include "KGParserDefines.h"

namespace kgx
{
	class RenderableObject;

	class KGObjectParser
	{
		public:
			static RenderableObject* loadKGO( const std::string &kgoFile );

		private:
			KGObjectParser();
			~KGObjectParser();

			// no copying allowed
			KGObjectParser( const KGObjectParser& );
			KGObjectParser& operator=(const KGObjectParser&);

			static RenderableObject* renderableObjectFromParseData( std::vector<float> vertices, std::vector<UINT> &indices,
																	std::vector<VertexInputLayout::Type> &vertLayoutTypes,
																	std::vector<KgModelData> &models,
																	std::map<std::string, KgMatData> &materials );
	};
}
