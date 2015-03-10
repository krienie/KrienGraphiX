
#pragma once

#include <map>

#include "parser_defines.h"

namespace kgx
{
	class OBJParser
	{
		public:
			struct FaceData
			{
				std::string groupName;
				std::string usemtl;
				std::vector<UINT> posIdx;
				std::vector<UINT> normIdx;
				std::vector<UINT> texIdx;
			};

			struct ObjParseData
			{
				std::string mtllib;
				std::vector<float> posCoords;
				std::vector<float> normCoords;
				std::vector<float> texCoords;
				std::vector<FaceData> faces;
			};

			struct ObjMatData
			{
				ObjMatData() : name(), specPwr(0.0f), optDens(0.0f), trans(1.0f),
					transFilter(), ambClr(), diffClr(), specClr(), emmisClr(),
					ambMap(), diffMap(), specMap(), transMap(), bumpMap()
				{
				}

				std::string name;
				float specPwr;
				float optDens;
				float trans;
				std::vector<float> transFilter;
				std::vector<float> ambClr;
				std::vector<float> diffClr;
				std::vector<float> specClr;
				std::vector<float> emmisClr;
				std::string ambMap;
				std::string diffMap;
				std::string specMap;
				std::string transMap;
				std::string bumpMap;
			};


			OBJParser();
			//~OBJParser();

			HRESULT parseFile( _In_ const std::string &fileName, _In_ const std::string &fileDir, _Inout_ KgmData &outputData );

		private:
			struct VertexData
			{
				VertexData(UINT pos, UINT norm, UINT tex)
					: posIdx(pos), normIdx(norm), texIdx(tex) {}

				UINT posIdx;
				UINT normIdx;
				UINT texIdx;

				inline bool operator<(const VertexData &rhs) const
				{
					return std::tie(posIdx, normIdx, texIdx) < std::tie(rhs.posIdx, rhs.normIdx, rhs.texIdx);
				}

				inline bool operator==(const VertexData &rhs) const
				{
					return std::tie(posIdx, normIdx, texIdx) == std::tie(rhs.posIdx, rhs.normIdx, rhs.texIdx);
				}
			};

			bool parseObj( const std::string &input, ObjParseData &objData );
			bool parseMtl( const std::string &input, std::vector<ObjMatData> &mtlData );
			void storeModelData( std::string modelName, ModelData &data,
				std::map< std::string, std::vector<ModelData> > *sortedModels, std::vector<ModelData*> *tempModels );
			int findIndex( const std::vector<VertexData> &vec, const VertexData &val ) const;
	};
}
