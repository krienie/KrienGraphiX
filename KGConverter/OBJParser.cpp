
#pragma warning( disable : 4244 )		// warning C4244: conversion from 'const double' to 'const float', possible loss of data. This is intentional.

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif
#include "boost/spirit/include/qi.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/support_iso8859_1.hpp"

#include "OBJParser.h"


BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT3,
	(float, x)
	(float, y)
	(float, z)
);


namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
using namespace qi;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
	OBJParser::OBJParser()
	{
	}

	//~OBJParser()


	bool OBJParser::parseFile( _In_ const std::string &file, _In_ const std::string &fileDir, _Inout_ KgoData &outputData )
	{
		std::stringstream objFile;
		objFile << fileDir << "/" << file;

		std::stringstream ssObj;
		std::filebuf fb;
		if ( fb.open( objFile.str(), std::ios::in ) )
		{
			ssObj << &fb;
			fb.close();
		} else
		{
			std::cout << "Error: Unable to open file " << file << '.' << std::endl;
			return false;
		}

		ObjParseData objData;
		if (!parseObj(ssObj.str(), objData))
		{
			std::cout << "Error: OBJ parsing failed." << std::endl;
			return false;
		}

		// only attempt to load a MTL file when one is mentioned in the OBJ file
		std::vector<ObjMatData> objMats;
		if ( objData.mtllib.size() > 0 )
		{
			std::stringstream mtlFile;
			mtlFile << fileDir << "/" << objData.mtllib;

			std::stringstream ssMtl;
			if ( fb.open( mtlFile.str(), std::ios::in ) )
			{
				ssMtl << &fb;
				fb.close();
			} else
			{
				std::cout << "Error: Unable to open file " << objData.mtllib << '.' << std::endl;
				return false;
			}

			if ( !parseMtl(ssMtl.str(), objMats) )
			{
				std::cout << "Error: MTL parsing failed." << std::endl;
				return false;
			}
		} else
		{
			// add default material
			objMats.push_back( ObjMatData() );
			ObjMatData &objMatData = objMats[objMats.size() - 1u];
			objMatData.name = "DefaultMaterial";

			float red   = rand() / (float)RAND_MAX;
			float green = rand() / (float)RAND_MAX;
			float blue  = rand() / (float)RAND_MAX;
			objMatData.diffClr = DirectX::XMFLOAT3( red, green, blue );

			// set default material for the first object
			if ( objData.faces.size() > 0 )
				objData.faces[0].usemtl = objMatData.name;
			// set default material for the other objects, if there are some
			std::vector<FaceData>::iterator it;
			for ( it = objData.faces.begin(); it != objData.faces.end(); ++it )
				if ( it->groupName.size() > 0 )
					it->usemtl = objMatData.name;
		}


		// Convert ObjMatData to KgMatData
		std::vector<ObjMatData>::iterator it;
		for ( it = objMats.begin(); it != objMats.end(); ++it )
		{
			outputData.mats.push_back( KgMatData() );
			KgMatData &mData = outputData.mats[outputData.mats.size() - 1u];
			convertToMatData( *it, mData );
		}


		if (objData.normCoords.size() == 0)
		{
			objData.normCoords.push_back(0.0f);
			objData.normCoords.push_back(0.0f);
			objData.normCoords.push_back(0.0f);
		}

		if (objData.texCoords.size() == 0)
		{
			objData.texCoords.push_back(0.0f);
			objData.texCoords.push_back(0.0f);
			objData.texCoords.push_back(0.0f);
		}

		// process parsed OBJ data to a format KrienGraphiX can work with
		processModelData( objData, outputData );


		// register Vertex layout
		outputData.inputLayout.push_back( kgx::VertexInputLayout::Position );
		outputData.inputLayout.push_back( kgx::VertexInputLayout::TextureCoordinate );
		outputData.inputLayout.push_back( kgx::VertexInputLayout::Normal );

		return true;
	}


	bool OBJParser::parseObj( const std::string &input, ObjParseData &objData )
	{
		//TODO: add support for smoothing groups

		struct ObjGram : qi::grammar<std::string::const_iterator, ObjParseData(), Skipper>
		{
			ObjGram() : ObjGram::base_type(start)
			{
				skipLine = ((lit('#') | lit('s') | lit('o')) >> qi::skip(qi::blank)[*qi::print]);
				defaultGroup = lit('g') >> qi::skip(qi::blank)[*qi::print];

				auto setMatlib = phx::push_back(phx::bind(&ObjParseData::mtllib, _r1), qi::_1);
				matlib = "mtllib" >> qi::skip(qi::blank)[*qi::print[setMatlib]];
				vertex = -defaultGroup >> 'v' >> repeat(3)[double_[phx::push_back(phx::bind(&ObjParseData::posCoords, _r1), qi::_1)]];
				normal = "vn" >> repeat(3)[double_[phx::push_back(phx::bind(&ObjParseData::normCoords, _r1), qi::_1)]
					| eps[phx::push_back(phx::bind(&ObjParseData::normCoords, _r1), 0.0f)]];
				texture = "vt" >> repeat(3)[double_[phx::push_back(phx::bind(&ObjParseData::texCoords, _r1), qi::_1)]
					| eps[phx::push_back(phx::bind(&ObjParseData::texCoords, _r1), 0.0f)]];

				auto addPosIdx  = phx::push_back(phx::bind(&FaceData::posIdx, _r1), qi::_1);
				auto addTexIdx  = phx::push_back(phx::bind(&FaceData::texIdx, _r1), qi::_1);
				auto addNormIdx = phx::push_back(phx::bind(&FaceData::normIdx, _r1), qi::_1);

				auto setGroupName = phx::push_back(phx::bind(&FaceData::groupName, _r1), qi::_1);
				auto setUsemtl    = phx::push_back(phx::bind(&FaceData::usemtl, _r1), qi::_1);
				groupName = "g" >> qi::skip(qi::blank)[*qi::print[setGroupName]];
				usemtl    = "usemtl" >> qi::skip(qi::blank)[*qi::print[setUsemtl]];

				face = 'f' >> *(int_[addPosIdx]
								>> no_skip[-lit('/') >> (int_[addTexIdx] | eps[phx::push_back(phx::bind(&FaceData::texIdx, _r1), 0)])
								>> -lit('/') >> (int_[addNormIdx] | eps[phx::push_back(phx::bind(&FaceData::normIdx, _r1), 0)])]);

				elem     = -groupName(_val) >> -usemtl(_val) >> face(_val);
				elements = elem[phx::push_back(phx::bind(&ObjParseData::faces, _r1), qi::_1)];

				start = *(matlib(_val) | vertex(_val) | normal(_val) | texture(_val) | elements(_val));
			}

			qi::rule<std::string::const_iterator, ObjParseData(), Skipper> start;
			qi::rule<std::string::const_iterator, void(ObjParseData&), Skipper> matlib, vertex, normal, texture, elements;
			qi::rule<std::string::const_iterator, void(FaceData&), Skipper> face, groupName, usemtl;
			qi::rule<std::string::const_iterator, FaceData(), Skipper> elem;
			Skipper skipLine, defaultGroup;
		} objGrammar;

		Skipper skipper = iso8859::space | objGrammar.skipLine;

		std::string::const_iterator f = input.cbegin();
		bool res = qi::phrase_parse(f, input.cend(), objGrammar, skipper, objData);

		// print everything that hasn't been processed by the parser
		if (f != input.cend())
		{
			std::string::const_iterator end = std::distance(f, input.cend()) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "Obj parsing trail: " << std::string(f, end) << std::endl;
			return false;
		}

		return res;
	}


	bool OBJParser::parseMtl(const std::string &input, std::vector<ObjMatData> &mtlData)
	{
		struct mtlGram : qi::grammar<std::string::const_iterator, std::vector<ObjMatData>(), Skipper>
		{
			mtlGram() : mtlGram::base_type(start)
			{
				skipLine = (lit('#') | lit("illum")) >> qi::skip(qi::blank)[*qi::print];

				specPwr = "Ns" >> double_[phx::bind(&ObjMatData::specPwr, _r1) = qi::_1];
				optDens = "Ni" >> double_[phx::bind(&ObjMatData::optDens, _r1) = qi::_1];
				transD  = 'd'  >> double_[phx::bind(&ObjMatData::trans, _r1) = qi::_1];
				transTr = "Tr" >> *double_[phx::bind(&ObjMatData::trans, _r1) = 1.0f - qi::_1];

				auto addNewmtl   = phx::push_back(phx::bind(&ObjMatData::name, _r1), qi::_1);
				auto addAmbMap   = phx::push_back(phx::bind(&ObjMatData::ambMap, _r1), qi::_1);
				auto addDiffMap  = phx::push_back(phx::bind(&ObjMatData::diffMap, _r1), qi::_1);
				auto addSpecMap  = phx::push_back(phx::bind(&ObjMatData::specMap, _r1), qi::_1);
				auto addTransMap = phx::push_back(phx::bind(&ObjMatData::transMap, _r1), qi::_1);
				auto addBumpMap  = phx::push_back(phx::bind(&ObjMatData::bumpMap, _r1), qi::_1);

				newmtl   = "newmtl" >> qi::skip(qi::blank)[*qi::print[addNewmtl]];
				ambMap   = "map_Ka" >> qi::skip(qi::blank)[*qi::print[addAmbMap]];
				diffMap  = "map_Kd" >> qi::skip(qi::blank)[*qi::print[addDiffMap]];
				specMap  = (lit("map_Ks") | lit("map_refl")) >> qi::skip(qi::blank)[*qi::print[addSpecMap]];
				transMap = "map_d" >> qi::skip(qi::blank)[*qi::print[addTransMap]];
				bumpMap  = (lit("map_bump") | lit("bump")) >> eps[phx::resize(phx::bind(&ObjMatData::bumpMap, _r1), 0)]
					>> lexeme[*(print - iso8859::space)[addBumpMap]] >> qi::skip(qi::blank)[*qi::print];

				xmFloat3 = float_ >> float_ >> float_;
				transFilter = "Tf" >> xmFloat3[phx::bind(&ObjMatData::transFilter, _r1) = qi::_1];
				ambClr      = "Ka" >> xmFloat3[phx::bind( &ObjMatData::ambClr, _r1 ) = qi::_1];
				diffClr     = "Kd" >> xmFloat3[phx::bind( &ObjMatData::diffClr, _r1 ) = qi::_1];
				specClr     = "Ks" >> xmFloat3[phx::bind( &ObjMatData::specClr, _r1 ) = qi::_1];
				emmisClr    = "Ke" >> xmFloat3[phx::bind( &ObjMatData::emmisClr, _r1 ) = qi::_1];

				material = newmtl(_val) >> *(specPwr(_val) | optDens(_val) | transD(_val) | transTr(_val)
											 | transFilter(_val) | ambClr(_val) | diffClr(_val) | specClr(_val) | emmisClr(_val)
											 | ambMap(_val) | diffMap(_val) | specMap(_val) | transMap(_val) | bumpMap(_val) | skipLine);

				start = *material;
			}

			qi::rule<std::string::const_iterator, std::vector<ObjMatData>(), Skipper> start;
			qi::rule<std::string::const_iterator, ObjMatData(), Skipper> material;
			qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> xmFloat3;
			qi::rule<std::string::const_iterator, void(ObjMatData&), Skipper> specPwr, optDens, transD, transTr;
			qi::rule<std::string::const_iterator, void(ObjMatData&), Skipper> transFilter, ambClr, diffClr, specClr, emmisClr;
			qi::rule<std::string::const_iterator, void(ObjMatData&), Skipper> newmtl, ambMap, diffMap, specMap, transMap;
			qi::rule<std::string::const_iterator, void(ObjMatData&), Skipper> bumpMap;
			Skipper skipLine;
		} mtlGrammar;

		Skipper skipper = iso8859::space | mtlGrammar.skipLine;

		std::string::const_iterator f = input.cbegin();
		bool res = qi::phrase_parse(f, input.cend(), mtlGrammar, skipper, mtlData);

		// print everything that hasn't been processed by the parser
		if (f != input.cend())
		{
			std::string::const_iterator end = std::distance(f, input.cend()) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "Mtl parsing trail: " << std::string(f, end) << std::endl;
			return false;
		}

		return res;
	}


	/**
	* utility function used for OBJ parsing
	*/
	void OBJParser::storeModelData( std::string modelName, KgModelData &data,
										std::map< std::string, std::vector<KgModelData> > *sortedModels, std::vector<KgModelData*> *tempModels )
	{
		typedef std::map< std::string, std::vector<KgModelData> >::iterator modelIt;
		modelIt mIt = sortedModels->find(modelName);

		if ( mIt != sortedModels->end() )
		{
			mIt->second.push_back(data);
		} else
		{
			std::pair<modelIt, bool> res = sortedModels->insert( std::pair< std::string, std::vector<KgModelData> >(modelName, std::vector<KgModelData>(1, data)) );
			mIt = res.first;
		}

		tempModels->push_back(&mIt->second[mIt->second.size() - 1]);
	}


	void OBJParser::processModelData( const ObjParseData &objData, KgoData &kgData )
	{
		std::map<VertexData, UINT> verts;
		std::map<UINT, VertexData> vertIdices;

		// these two below are needed to handle duplicate object names
		std::map< std::string, std::vector<KgModelData> > sortedModels;
		std::vector<KgModelData*> tempModels;

		UINT maxIdx = 0;
		std::vector<FaceData>::const_iterator i;
		for ( i = objData.faces.cbegin(); i != objData.faces.cend(); ++i )
		{
			//TODO: handle when a mtllib is defined, but not found, also for usemtl's

			if ( i->groupName.size() > 0 || i->usemtl.size() > 0 )		// faces with groupName or usemtl defined indicate a new model
			{
				// save index ranges for usemtl flags
				if ( tempModels.size() >= 1 )
					tempModels[tempModels.size() - 1]->indexCount =
					int( kgData.indices.size() ) - tempModels[tempModels.size() - 1]->startIndex;

				KgModelData newModel( i->groupName, i->usemtl, int( kgData.indices.size() ), 0 );
				storeModelData( i->groupName, newModel, &sortedModels, &tempModels );
			}

			std::vector<long int>::const_iterator posIt = i->posIdx.cbegin();
			std::vector<long int>::const_iterator texIt = i->texIdx.cbegin();
			std::vector<long int>::const_iterator normIt = i->normIdx.cbegin();

			VertexData firstVert( max(0, std::abs(*posIt) - 1), max(0, std::abs(*normIt) - 1), max(0, std::abs(*texIt) - 1) );				//OBJ indices start at 1 and we need to start at 0, so idx - 1
			UINT firstIdx = maxIdx++;
			verts.insert( verts.end(), std::pair<VertexData, UINT>( firstVert, firstIdx ) );
			vertIdices.insert( vertIdices.end(), std::pair<UINT, VertexData>( firstIdx, firstVert ) );

			//TODO: add support for concave polygons
			posIt++; normIt++; texIt++;
			int faceSize = (int)i->posIdx.size();
			for ( int j = 0; j < faceSize - 2; ++j )					// triangulate convex polygon
			{
				// add triangle vertices to unique-vertex list
				UINT endVertIdx = 0;
				VertexData endVert( max(0, std::abs(*posIt) - 1), max(0, std::abs(*normIt) - 1), max(0, std::abs(*texIt) - 1) );
				std::map<VertexData, UINT>::iterator vertIt;
				vertIt = verts.find( endVert );
				if ( vertIt == verts.end() )
				{
					endVertIdx = maxIdx++;
					verts.insert( verts.end(), std::pair<VertexData, UINT>( endVert, endVertIdx ) );
					vertIdices.insert( vertIdices.end(), std::pair<UINT, VertexData>( endVertIdx, endVert ) );
				} else
				{
					endVertIdx = vertIt->second;
				}

				UINT midVertIdx = 0;
				posIt++; normIt++; texIt++;
				VertexData midVert( max(0, std::abs(*posIt) - 1), max(0, std::abs(*normIt) - 1), max(0, std::abs(*texIt) - 1) );
				vertIt = verts.find( midVert );
				if ( vertIt == verts.end() )
				{
					midVertIdx = maxIdx++;
					verts.insert( verts.end(), std::pair<VertexData, UINT>( midVert, midVertIdx ) );
					vertIdices.insert( vertIdices.end(), std::pair<UINT, VertexData>( midVertIdx, midVert ) );
				} else
				{
					midVertIdx = vertIt->second;
				}

				// add indices for the triangle
				kgData.indices.push_back( firstIdx );
				kgData.indices.push_back( midVertIdx );
				kgData.indices.push_back( endVertIdx );
			}
		}

		//calculate index count for the last added KgModelData
		if ( tempModels.size() > 0 )
		{
			tempModels[tempModels.size() - 1]->indexCount =
				int( kgData.indices.size() ) - tempModels[tempModels.size() - 1]->startIndex;
		} else
		{
			KgModelData newModel( "", "", 0, int( kgData.indices.size() ) );
			storeModelData( "", newModel, &sortedModels, &tempModels );
		}

		// fill outputVerts using the set of FaceData structs.
		for ( UINT i = 0; i < maxIdx; ++i )
		{
			std::map<UINT, VertexData>::const_iterator vIt;
			vIt = vertIdices.find( i );

			// position coordinates
			kgData.vertices.push_back( objData.posCoords[vIt->second.posIdx * 3] );
			kgData.vertices.push_back( objData.posCoords[(vIt->second.posIdx * 3) + 1] );
			kgData.vertices.push_back( objData.posCoords[(vIt->second.posIdx * 3) + 2] );

			// texture UVW coordinates
			kgData.vertices.push_back( objData.texCoords[vIt->second.texIdx * 3] );
			kgData.vertices.push_back( 1.0f - objData.texCoords[(vIt->second.texIdx * 3) + 1] );
			kgData.vertices.push_back( objData.texCoords[(vIt->second.texIdx * 3) + 2] );

			// normal coordinates
			kgData.vertices.push_back( objData.normCoords[vIt->second.normIdx * 3] );
			kgData.vertices.push_back( objData.normCoords[(vIt->second.normIdx * 3) + 1] );
			kgData.vertices.push_back( objData.normCoords[(vIt->second.normIdx * 3) + 2] );
		}

		// assign correct name to each object
		std::map< std::string, std::vector<KgModelData> >::iterator mIt;
		for ( mIt = sortedModels.begin(); mIt != sortedModels.end(); ++mIt )
		{
			std::vector<KgModelData>::iterator i;
			if ( mIt->first.size() > 0 )
			{
				if ( mIt->second.size() == 1 )
				{
					kgData.models.push_back( mIt->second[0] );
				} else																	//make sure there are no duplicate object names
				{
					UINT nameCount = 1;
					for ( i = mIt->second.begin(); i != mIt->second.end(); ++i )
					{
						std::stringstream ssName;
						ssName << i->modelName << '_' << nameCount;
						nameCount++;

						i->modelName = ssName.str();
						kgData.models.push_back( *i );
					}
				}
			} else																		//give a name to unnamed objects
			{
				UINT unnamedCount = 1;
				for ( i = mIt->second.begin(); i != mIt->second.end(); ++i )
				{
					std::stringstream ssName;
					ssName << "UnnamedObject" << unnamedCount;
					unnamedCount++;

					i->modelName = ssName.str();
					kgData.models.push_back( *i );
				}
			}
		}
	}


	void OBJParser::convertToMatData( const ObjMatData &objMat, KgMatData &kgMat )
	{
		kgMat.name = objMat.name;

		
		//TODO: change to Default OBJ shaders
		kgMat.vertexShader.name = "DefaultVertexShaderVS.cso";
		kgMat.pixelShader.name  = "DefaultPixelShaderPS.cso";

		//vertex shader auto variables
		KgMatData::ShaderVar var;
		var.name         = "viewMatrix";
		var.type         = "mat4x4";
		var.autoBindType = Material::ShaderAutoBindType::CameraViewMatrix;
		kgMat.vertexShader.variables.push_back( var );

		var.name         = "projMatrix";
		var.type         = "mat4x4";
		var.autoBindType = Material::ShaderAutoBindType::CameraProjectionMatrix;
		kgMat.vertexShader.variables.push_back( var );

		var.name         = "modelMatrix";
		var.type         = "mat4x4";
		var.autoBindType = Material::ShaderAutoBindType::ObjectModelMatrix;
		kgMat.vertexShader.variables.push_back( var );

		// pixel shader variables
		//TODO: add pixel shader variables

		//TODO: add textures
	}
}

#pragma warning( default : 4244 )	// restore default warning level
