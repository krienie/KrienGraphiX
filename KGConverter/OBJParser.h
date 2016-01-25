
#pragma once

#include <DirectXMath.h>
#include <map>

#include <KGParserDefines.h>

namespace kgx
{
	class OBJParser
	{
		public:
			struct FaceData
			{
				FaceData() : groupName(), usemtl(), posIdx(), normIdx(), texIdx() {}

				std::string groupName;
				std::string usemtl;
				std::vector<long int> posIdx;
				std::vector<long int> normIdx;
				std::vector<long int> texIdx;
			};

			struct ObjParseData
			{
				ObjParseData() : mtllib(), posCoords(), normCoords(), texCoords(), faces() {}

				std::string mtllib;
				std::vector<float> posCoords;
				std::vector<float> normCoords;
				std::vector<float> texCoords;
				std::vector<FaceData> faces;
			};

			struct ObjMatData
			{
				ObjMatData() : name(), specPwr(0.0f), optDens(0.0f), trans(1.0f),
					transFilter(0.0f, 0.0f, 0.0f), ambClr(0.0f, 0.0f, 0.0f), diffClr(0.0f, 0.0f, 0.0f),
					specClr(0.0f, 0.0f, 0.0f), emmisClr(0.0f, 0.0f, 0.0f),
					ambMap(), diffMap(), specMap(), transMap(), bumpMap()
				{
				}

				std::string name;
				float specPwr;
				float optDens;
				float trans;
				DirectX::XMFLOAT3 transFilter;
				DirectX::XMFLOAT3 ambClr;
				DirectX::XMFLOAT3 diffClr;
				DirectX::XMFLOAT3 specClr;
				DirectX::XMFLOAT3 emmisClr;
				std::string ambMap;
				std::string diffMap;
				std::string specMap;
				std::string transMap;
				std::string bumpMap;
			};


			OBJParser();
			//~OBJParser();

			bool parseFile( const std::string &file, const std::string &fileDir, KgoData &outputData );

		private:
			struct VertexData
			{
				VertexData( UINT pos, UINT norm, UINT tex )
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
			void storeModelData( std::string modelName, KgModelData &data,
				std::map< std::string, std::vector<KgModelData> > *sortedModels, std::vector<KgModelData*> *tempModels );
			void processModelData( const ObjParseData &objData, KgoData &kgData );
			void convertToMatData( const ObjMatData &objMat, KgMatData &kgMat );
	};
}
