
#pragma warning( disable : 4244 )		// warning C4244: conversion from 'const double' to 'const float', possible loss of data. This is intentional.

#define NOMINMAX

#include "OBJParser.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_iso8859_1.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    DirectX::XMFLOAT3,
    (float, x)
    (float, y)
    (float, z)
);

namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
    OBJParser::OBJParser()
    {
    }

    //~OBJParser()


    bool OBJParser::parseFile( const std::string &file, const std::string &fileDir, KgoData &outputData )
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
        if ( !parseObj( ssObj.str(), objData ) )
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

            if ( !parseMtl( ssMtl.str(), objMats ) )
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

            float red = rand() / (float)RAND_MAX;
            float green = rand() / (float)RAND_MAX;
            float blue = rand() / (float)RAND_MAX;
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


        if ( objData.normCoords.size() == 0 )
        {
            objData.normCoords.push_back( 0.0f );
            objData.normCoords.push_back( 0.0f );
            objData.normCoords.push_back( 0.0f );
        }

        if ( objData.texCoords.size() == 0 )
        {
            objData.texCoords.push_back( 0.0f );
            objData.texCoords.push_back( 0.0f );
            objData.texCoords.push_back( 0.0f );
        }

        // process parsed OBJ data to a format KrienGraphiX can work with
        processModelData( objData, outputData );


        // register Vertex layout
        outputData.inputLayout.push_back( kgx::VertexInputLayout::Type::Position );
        outputData.inputLayout.push_back( kgx::VertexInputLayout::Type::TextureCoordinate );
        outputData.inputLayout.push_back( kgx::VertexInputLayout::Type::Normal );

        return true;
    }


    bool OBJParser::parseObj( const std::string &input, ObjParseData &objData )
    {
        //TODO: add support for smoothing groups

        struct ObjGram : qi::grammar<std::string::const_iterator, ObjParseData(), Skipper>
        {
            ObjGram() : base_type( start )
            {
                skipLine = (qi::lit( '#' ) | qi::lit( 's' ) | qi::lit( 'o' )) >> qi::skip( qi::blank )[*qi::print];
                defaultGroup = qi::lit( 'g' ) >> qi::skip( qi::blank )[*qi::print];

                auto setMatlib = phx::push_back( phx::bind( &ObjParseData::mtllib, qi::_r1 ), qi::_1 );
                matlib = "mtllib" >> qi::skip( qi::blank )[*qi::print[setMatlib]];
                vertex = -defaultGroup >> 'v' >> qi::repeat( 3 )[qi::double_[phx::push_back( phx::bind( &ObjParseData::posCoords, qi::_r1 ), qi::_1 )]];
                normal = "vn" >> qi::repeat( 3 )[qi::double_[phx::push_back( phx::bind( &ObjParseData::normCoords, qi::_r1 ), qi::_1 )]
                    | qi::eps[phx::push_back( phx::bind( &ObjParseData::normCoords, qi::_r1 ), 0.0f )]];
                texture = "vt" >> qi::repeat( 3 )[qi::double_[phx::push_back( phx::bind( &ObjParseData::texCoords, qi::_r1 ), qi::_1 )]
                    | qi::eps[phx::push_back( phx::bind( &ObjParseData::texCoords, qi::_r1 ), 0.0f )]];

                auto addPosIdx = phx::push_back( phx::bind( &FaceData::posIdx, qi::_r1 ), qi::_1 );
                auto addTexIdx = phx::push_back( phx::bind( &FaceData::texIdx, qi::_r1 ), qi::_1 );
                auto addNormIdx = phx::push_back( phx::bind( &FaceData::normIdx, qi::_r1 ), qi::_1 );

                auto setGroupName = phx::push_back( phx::bind( &FaceData::groupName, qi::_r1 ), qi::_1 );
                auto setUsemtl = phx::push_back( phx::bind( &FaceData::usemtl, qi::_r1 ), qi::_1 );
                groupName = "g" >> qi::skip( qi::blank )[*qi::print[setGroupName]];
                usemtl = "usemtl" >> qi::skip( qi::blank )[*qi::print[setUsemtl]];

                face = 'f' >> *(qi::int_[addPosIdx]
                                 >> qi::no_skip[-qi::lit( '/' ) >> (qi::int_[addTexIdx] | qi::eps[phx::push_back( phx::bind( &FaceData::texIdx, qi::_r1 ), 0 )])
                                 >> -qi::lit( '/' ) >> (qi::int_[addNormIdx] | qi::eps[phx::push_back( phx::bind( &FaceData::normIdx, qi::_r1 ), 0 )])]);

                elem = -groupName( qi::_val ) >> -usemtl( qi::_val ) >> face( qi::_val );
                elements = elem[phx::push_back( phx::bind( &ObjParseData::faces, qi::_r1 ), qi::_1 )];

                start = *(matlib( qi::_val ) | vertex( qi::_val ) | normal( qi::_val ) | texture( qi::_val ) | elements( qi::_val ));
            }

            qi::rule<std::string::const_iterator, ObjParseData(), Skipper> start;
            qi::rule<std::string::const_iterator, void( ObjParseData& ), Skipper> matlib, vertex, normal, texture, elements;
            qi::rule<std::string::const_iterator, void( FaceData& ), Skipper> face, groupName, usemtl;
            qi::rule<std::string::const_iterator, FaceData(), Skipper> elem;
            Skipper skipLine, defaultGroup;
        } objGrammar;

        Skipper skipper = iso8859::space | objGrammar.skipLine;

        std::string::const_iterator f = input.cbegin();
        bool res = qi::phrase_parse( f, input.cend(), objGrammar, skipper, objData );

        // print everything that hasn't been processed by the parser
        if ( f != input.cend() )
        {
            std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
            std::cout << std::endl << "Obj parsing trail: " << std::string( f, end ) << std::endl;
            return false;
        }

        return res;
    }


    bool OBJParser::parseMtl( const std::string &input, std::vector<ObjMatData> &mtlData )
    {
        struct mtlGram : qi::grammar<std::string::const_iterator, std::vector<ObjMatData>(), Skipper>
        {
            mtlGram() : base_type( start )
            {
                skipLine = (qi::lit( '#' ) | qi::lit( "illum" )) >> qi::skip( qi::blank )[*qi::print];

                specPwr = "Ns" >> qi::double_[phx::bind( &ObjMatData::specPwr, qi::_r1 ) = qi::_1];
                optDens = "Ni" >> qi::double_[phx::bind( &ObjMatData::optDens, qi::_r1 ) = qi::_1];
                transD = 'd' >> qi::double_[phx::bind( &ObjMatData::trans, qi::_r1 ) = qi::_1];
                transTr = "Tr" >> *qi::double_[phx::bind( &ObjMatData::trans, qi::_r1 ) = 1.0f - qi::_1];

                auto addNewmtl = phx::push_back( phx::bind( &ObjMatData::name, qi::_r1 ), qi::_1 );
                auto addAmbMap = phx::push_back( phx::bind( &ObjMatData::ambMap, qi::_r1 ), qi::_1 );
                auto addDiffMap = phx::push_back( phx::bind( &ObjMatData::diffMap, qi::_r1 ), qi::_1 );
                auto addSpecMap = phx::push_back( phx::bind( &ObjMatData::specMap, qi::_r1 ), qi::_1 );
                auto addTransMap = phx::push_back( phx::bind( &ObjMatData::transMap, qi::_r1 ), qi::_1 );
                auto addBumpMap = phx::push_back( phx::bind( &ObjMatData::bumpMap, qi::_r1 ), qi::_1 );

                newmtl = "newmtl" >> qi::skip( qi::blank )[*qi::print[addNewmtl]];
                ambMap = "map_Ka" >> qi::no_skip[*qi::print[addAmbMap]];
                diffMap = "map_Kd" >> qi::no_skip[*qi::print[addDiffMap]];
                specMap = (qi::lit( "map_Ks" ) | qi::lit( "map_refl" ) | qi::lit( "map_Ns" )) >> qi::no_skip[*qi::print[addSpecMap]];
                transMap = "map_d" >> qi::no_skip[*qi::print[addTransMap]];
                bumpMap = (qi::lit( "map_bump" ) | qi::lit( "bump" )) >> qi::eps[phx::resize( phx::bind( &ObjMatData::bumpMap, qi::_r1 ), 0 )]
                    >> qi::no_skip[*(qi::print)[addBumpMap]];

                xmFloat3 = qi::float_ >> qi::float_ >> qi::float_;
                transFilter = "Tf" >> xmFloat3[phx::bind( &ObjMatData::transFilter, qi::_r1 ) = qi::_1];
                ambClr = "Ka" >> xmFloat3[phx::bind( &ObjMatData::ambClr, qi::_r1 ) = qi::_1];
                diffClr = "Kd" >> xmFloat3[phx::bind( &ObjMatData::diffClr, qi::_r1 ) = qi::_1];
                specClr = "Ks" >> xmFloat3[phx::bind( &ObjMatData::specClr, qi::_r1 ) = qi::_1];
                emmisClr = "Ke" >> xmFloat3[phx::bind( &ObjMatData::emmisClr, qi::_r1 ) = qi::_1];

                material = newmtl( qi::_val ) >> *(specPwr( qi::_val ) | optDens( qi::_val ) | transD( qi::_val ) | transTr( qi::_val )
                                                    | transFilter( qi::_val ) | ambClr( qi::_val ) | diffClr( qi::_val ) | specClr( qi::_val ) | emmisClr( qi::_val )
                                                    | ambMap( qi::_val ) | diffMap( qi::_val ) | specMap( qi::_val ) | transMap( qi::_val ) | bumpMap( qi::_val ) | skipLine);

                start = *material;
            }

            qi::rule<std::string::const_iterator, std::vector<ObjMatData>(), Skipper> start;
            qi::rule<std::string::const_iterator, ObjMatData(), Skipper> material;
            qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> xmFloat3;
            qi::rule<std::string::const_iterator, void( ObjMatData& ), Skipper> specPwr, optDens, transD, transTr;
            qi::rule<std::string::const_iterator, void( ObjMatData& ), Skipper> transFilter, ambClr, diffClr, specClr, emmisClr;
            qi::rule<std::string::const_iterator, void( ObjMatData& ), Skipper> newmtl, ambMap, diffMap, specMap, transMap;
            qi::rule<std::string::const_iterator, void( ObjMatData& ), Skipper> bumpMap;
            Skipper skipLine;
        } mtlGrammar;

        Skipper skipper = iso8859::space | mtlGrammar.skipLine;

        std::string::const_iterator f = input.cbegin();
        bool res = qi::phrase_parse( f, input.cend(), mtlGrammar, skipper, mtlData );

        // print everything that hasn't been processed by the parser
        if ( f != input.cend() )
        {
            std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
            std::cout << std::endl << "Mtl parsing trail: " << std::string( f, end ) << std::endl;
            return false;
        }

        return res;
    }


    /**
    * utility function used for OBJ parsing
    */
    void OBJParser::storeModelData( std::string modelName, KgModelData &data,
                                    std::unordered_map<std::string, std::vector<KgModelData>> *sortedModels, std::vector<KgModelData*> *tempModels )
    {
        typedef std::unordered_map< std::string, std::vector<KgModelData> >::iterator modelIt;
        modelIt mIt = sortedModels->find( modelName );

        if ( mIt != sortedModels->end() )
        {
            mIt->second.push_back( data );
        } else
        {
            std::pair<modelIt, bool> res = sortedModels->insert( std::pair< std::string, std::vector<KgModelData> >( modelName, std::vector<KgModelData>( 1, data ) ) );
            mIt = res.first;
        }

        tempModels->push_back( &mIt->second[mIt->second.size() - 1] );
    }


    void OBJParser::processModelData( const ObjParseData &objData, KgoData &kgData )
    {
        std::map<VertexData, UINT> verts;
        std::unordered_map<UINT, VertexData> vertIdices;

        // these two below are needed to handle duplicate object names
        std::unordered_map< std::string, std::vector<KgModelData> > sortedModels;
        std::vector<KgModelData*> tempModels;

        UINT maxIdx = 0;
        for ( auto &face : objData.faces )
        {
            //TODO: handle when a mtllib is defined, but not found, also for usemtl's

            if ( face.groupName.size() > 0 || face.usemtl.size() > 0 )		// faces with groupName or usemtl defined indicate a new model
            {
                // save index ranges for usemtl flags
                if ( tempModels.size() >= 1 )
                {
                    tempModels[tempModels.size() - 1]->indexCount =
                        int( kgData.indices.size() ) - tempModels[tempModels.size() - 1]->startIndex;
                }

                KgModelData newModel( face.groupName, face.usemtl, int( kgData.indices.size() ), 0 );
                storeModelData( face.groupName, newModel, &sortedModels, &tempModels );
            }

            std::vector<long>::const_iterator posIt = face.posIdx.cbegin();
            std::vector<long>::const_iterator texIt = face.texIdx.cbegin();
            std::vector<long>::const_iterator normIt = face.normIdx.cbegin();

            UINT firstIdx = 0u;
            //OBJ indices start at 1 and we need to start at 0, so idx - 1
            VertexData firstVert( std::max( long( 0 ), std::abs( *posIt ) - 1 ), std::max( long( 0 ), std::abs( *normIt ) - 1 ), std::max( long( 0 ), std::abs( *texIt ) - 1 ) );
            std::map<VertexData, UINT>::iterator vertIt;
            vertIt = verts.find( firstVert );
            if ( vertIt == verts.end() )
            {
                firstIdx = maxIdx++;
                verts.insert( verts.end(), std::pair<VertexData, UINT>( firstVert, firstIdx ) );
                vertIdices.insert( vertIdices.end(), std::pair<UINT, VertexData>( firstIdx, firstVert ) );
            } else
            {
                firstIdx = vertIt->second;
            }

            //TODO: add support for concave polygons
            ++posIt; ++normIt; ++texIt;
            int faceSize = static_cast<int>(face.posIdx.size());
            for ( int j = 0; j < faceSize - 2; ++j )					// triangulate convex polygon
            {
                // add triangle vertices to unique-vertex list
                UINT endVertIdx = 0u;
                VertexData endVert( std::max( long( 0 ), std::abs( *posIt ) - 1 ), std::max( long( 0 ), std::abs( *normIt ) - 1 ), std::max( long( 0 ), std::abs( *texIt ) - 1 ) );
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

                UINT midVertIdx = 0u;
                ++posIt; ++normIt; ++texIt;
                VertexData midVert( std::max( long( 0 ), std::abs( *posIt ) - 1 ), std::max( long( 0 ), std::abs( *normIt ) - 1 ), std::max( long( 0 ), std::abs( *texIt ) - 1 ) );
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
            std::unordered_map<UINT, VertexData>::const_iterator vIt;
            vIt = vertIdices.find( i );

            // position coordinates
            kgData.vertices.push_back( objData.posCoords[vIt->second.posIdx * 3] );
            kgData.vertices.push_back( objData.posCoords[(vIt->second.posIdx * 3) + 1] );
            kgData.vertices.push_back( objData.posCoords[(vIt->second.posIdx * 3) + 2] );

            // texture UVW coordinates
            kgData.vertices.push_back( objData.texCoords[vIt->second.texIdx * 3] );
            kgData.vertices.push_back( 1.0f - objData.texCoords[(vIt->second.texIdx * 3) + 1] );		// flip v-coordinate
            kgData.vertices.push_back( objData.texCoords[(vIt->second.texIdx * 3) + 2] );

            // normal coordinates
            kgData.vertices.push_back( objData.normCoords[vIt->second.normIdx * 3] );
            kgData.vertices.push_back( objData.normCoords[(vIt->second.normIdx * 3) + 1] );
            kgData.vertices.push_back( objData.normCoords[(vIt->second.normIdx * 3) + 2] );
        }

        // assign correct name to each object
        for ( auto &sortedModelPair : sortedModels )
        {
            if ( sortedModelPair.first.size() > 0 )
            {
                if ( sortedModelPair.second.size() == 1 )
                {
                    kgData.models.push_back( sortedModelPair.second[0] );
                } else																	//make sure there are no duplicate object names
                {
                    UINT nameCount = 1;
                    for ( auto &sortedModel : sortedModelPair.second )
                    {
                        std::stringstream ssName;
                        ssName << sortedModel.modelName << '_' << nameCount;
                        nameCount++;

                        sortedModel.modelName = ssName.str();
                        kgData.models.push_back( sortedModel );
                    }
                }
            } else																		//give a name to unnamed objects
            {
                UINT unnamedCount = 1;
                for ( auto &unnamedModel : sortedModelPair.second )
                {
                    std::stringstream ssName;
                    ssName << "UnnamedObject" << unnamedCount;
                    unnamedCount++;

                    unnamedModel.modelName = ssName.str();
                    kgData.models.push_back( unnamedModel );
                }
            }
        }
    }


    void OBJParser::convertToMatData( const ObjMatData &objMat, KgMatData &kgMat )
    {
        kgMat.name = objMat.name;

        kgMat.diffuse.x = objMat.diffClr.x;
        kgMat.diffuse.y = objMat.diffClr.y;
        kgMat.diffuse.z = objMat.diffClr.z;
        kgMat.diffuse.w = 1.0f;

        kgMat.specular.x = objMat.specClr.x;
        kgMat.specular.y = objMat.specClr.y;
        kgMat.specular.z = objMat.specClr.z;
        kgMat.specular.w = 1.0f;

        kgMat.diffuseMap  = std::filesystem::path( objMat.diffMap ).filename().string();
        kgMat.specularMap = std::filesystem::path( objMat.specMap ).filename().string();
        kgMat.normalMap   = std::filesystem::path( objMat.bumpMap ).filename().string();
    }
}

#pragma warning( default : 4244 )	// restore default warning level
