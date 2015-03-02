
#pragma once

#include <tuple>

#include "Parser.h"


class OBJParser : public Parser
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


		OBJParser();
		//~OBJParser();

		HRESULT parseFile( _In_ const std::string &fileName, _In_ const std::string &fileDir, _Inout_opt_ KgmData *outputData );

	private:
		struct VertexData
		{
			VertexData( UINT pos, UINT norm, UINT tex )
				: posIdx(pos), normIdx(norm), texIdx(tex) {}

			UINT posIdx;
			UINT normIdx;
			UINT texIdx;

			inline bool operator<( const VertexData &rhs ) const
			{
				return std::tie(posIdx, normIdx, texIdx) < std::tie(rhs.posIdx, rhs.normIdx, rhs.texIdx);
			}

			inline bool operator==( const VertexData &rhs ) const
			{
				return std::tie(posIdx, normIdx, texIdx) == std::tie(rhs.posIdx, rhs.normIdx, rhs.texIdx);
			}
		};

		bool parseObj( const std::string &input, ObjParseData &objData );
		bool parseMtl( const std::string &input, std::vector<MatData> &mtlData );
		void storeMeshData( std::string modelName, MeshData &data, 
								std::map< std::string, std::vector<MeshData> > *sortedModels, std::vector<MeshData*> *tempModels );
		int findIndex( const std::vector<VertexData> &vec, const VertexData &val ) const;
};
