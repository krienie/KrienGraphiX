
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <Windows.h>
#include <string>
#include <vector>


struct MeshData
{
	MeshData( int start, int count = 0 ) 
		: startIndex(start), indexCount(count) { }

	MeshData( const std::string &mName, const std::string &usemtl, int start, int count = 0 ) 
		: meshName(mName), matName(usemtl), startIndex(start), indexCount(count) { }

	std::string meshName;
	int startIndex;
	int indexCount;
	std::string matName;
	//TODO: material parameters
};


//TODO: for now, this will be for storing OBJ MTL data, later I might put some other parameters here
struct MatData
{
	MatData() : name(), specPwr(0.0f), optDens(0.0f), trans(1.0f),
		transFilter(), ambClr(), diffClr(), specClr(), emmisClr(),
		ambMap(), diffMap(), specMap(), transMap(), bumpMap() {}

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


struct KgmData
{
	std::vector<float> vertices;
	std::vector<UINT> indices;
	std::vector<MeshData> models;
	std::vector<MatData> mats;
};


class Parser
{
	public:
		Parser() { }
		virtual ~Parser() { }

		virtual HRESULT parseFile( _In_ const std::string &fileName, _In_ const std::string &fileDir, _Inout_opt_ KgmData *outputData ) = 0;
};
