
#pragma once

#include <string>

#include "KGParserDefines.h"

namespace kgx
{
	class Scene;

	class KGObjectParser
	{
		public:
			static void loadKGO( const std::string &kgoFile, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale, Scene *scene );

		private:
			KGObjectParser();
			~KGObjectParser();

			// no copying allowed
			KGObjectParser( const KGObjectParser& );
			KGObjectParser& operator=(const KGObjectParser&);

			static bool addParsedDataToScene( std::vector<float> vertices, std::vector<UINT> &indices,
												std::vector<VertexInputLayout::Type> &vertLayoutTypes,
												std::vector<KgModelData> &models,
												const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale,
												Scene *scene );
	};
}
