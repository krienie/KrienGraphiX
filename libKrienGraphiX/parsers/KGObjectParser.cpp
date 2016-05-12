
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#pragma warning( disable : 4244 )		// warning C4244: conversion from 'unsigned int' to 'float', possible loss of data. This is intentional.

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_iso8859_1.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/filesystem.hpp>

#include "KGParserDefines.h"
#include "../KGXCore.h"
#include "../RenderableObject.h"
#include "../VertexInputLayout.h"
#include "../Filesystem.h"
#include "../ResourceManager.h"
#include "../Texture.h"
#include "../TextureManager.h"

#include "KGObjectParser.h"

BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT4,
	(float, x)
	(float, y)
	(float, z)
	(float, w)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(DirectX::XMFLOAT4, diffuse)
	(DirectX::XMFLOAT4, specular)
	(std::string, diffuseMap)
	(std::string, specularMap)
	(std::string, normalMap)
);


namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
	RenderableObject* KGObjectParser::loadKGO( const std::string &kgoFile )
	{
		std::string absKGOFile = kgoFile;
		if ( !boost::filesystem::path(kgoFile).is_absolute() )
			absKGOFile = filesystem::getAbsolutePath(kgoFile);
		if ( absKGOFile.size() == 0 )
		{
			std::cout << "Error (KGObjectParser::loadKGO): Model source file not specified." << std::endl;
			return nullptr;
		}

		if ( !boost::filesystem::exists( absKGOFile ) )
		{
			std::cout << "Error (KGObjectParser::loadKGO): Model source file does not exist." << std::endl;
			return nullptr;
		}

		std::stringstream ssKgo;
		std::filebuf fb;
		if ( fb.open( absKGOFile, std::ios::in ) )
		{
			ssKgo << &fb;
			fb.close();
		}

		std::vector<float> vertices;
		std::vector<UINT> indices;
		std::vector<VertexInputLayout::Type> vertLayoutTypes;
		std::vector<KgModelData> models;
		std::map<std::string, KgMatData> materials;
		struct KgoGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			KgoGrammar( std::vector<float> &v, std::vector<UINT> &i, std::vector<VertexInputLayout::Type> &l,
						std::vector<KgModelData> &m, std::map<std::string, KgMatData> &mats )
				: KgoGrammar::base_type( start )
			{
				comment = "//" >> qi::skip(qi::blank)[*qi::print];

				vertexInputLayout = qi::string("Position")[qi::_val = VertexInputLayout::Position]
									| qi::string("TextureCoordinate")[qi::_val = VertexInputLayout::TextureCoordinate]
									| qi::string("Normal")[qi::_val = VertexInputLayout::Normal]
									| qi::string("Tangent")[qi::_val = VertexInputLayout::Tangent];
				vertices = "Vertices" >> qi::lit("(") >> vertexInputLayout[phx::push_back( phx::ref(l), qi::_1 )] % qi::char_(',') >> ")"
					>> qi::lit("{") >> *qi::float_[phx::push_back( phx::ref(v), qi::_1 )] >> "}";

				indices = "Indices" >> qi::lit("(") >> qi::lit(")") >> qi::lit("{") >> *qi::uint_[phx::push_back( phx::ref(i), qi::_1 )] >> "}";

				//TODO: add support for different order of model contents
				models = "Model" >> qi::lit("(") >> *~qi::char_(')') >> qi::lit(")") >> qi::lit("{")
					>> "Indices" >> qi::lit("(") >> qi::int_ >> qi::lit(",") >> qi::int_ >> qi::lit(")")
					>> "Material" >> qi::lit("(") >> *~qi::char_(')') >> qi::lit(")")
					>> qi::lit("}");

				float4Property = qi::lit("(") >> qi::float_ >> qi::lit(",") >> qi::float_ >> qi::lit(",") >> qi::float_ >> qi::lit(",") >> qi::float_ >> qi::lit(")");
				stringProperty = qi::lit("(") >> *~qi::char_(')') >> qi::lit(")");
				material = qi::lit("diffuse")     >> float4Property
						>> qi::lit("specular")    >> float4Property
						>> -(qi::lit("diffuseMap")  >> stringProperty)
						>> -(qi::lit("specularMap") >> stringProperty)
						>> -(qi::lit("normalMap")   >> stringProperty);
				nameMatPair = qi::lit("Material") >> qi::lit("(") >> *~qi::char_(')') >> qi::lit(")")
					>> qi::lit("{") >> material >> qi::lit("}");

				start = *(vertices | indices | models[phx::push_back( phx::ref(m), qi::_1 )]
						   | nameMatPair[phx::insert( phx::ref(mats), qi::_1 )]);
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, Skipper> vertices, indices;
			qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
			qi::rule<std::string::const_iterator, std::string(), Skipper> vertexInputLayoutName;

			qi::rule<std::string::const_iterator, KgModelData(), Skipper> models;

			qi::rule<std::string::const_iterator, DirectX::XMFLOAT4(), Skipper> float4Property;
			qi::rule<std::string::const_iterator, std::string(), Skipper> stringProperty;
			qi::rule<std::string::const_iterator, KgMatData(), Skipper> material;
			qi::rule<std::string::const_iterator, std::pair<std::string, KgMatData>(), Skipper> nameMatPair;

			qi::rule<std::string::const_iterator> comment;
		} kgmGrammar( vertices, indices, vertLayoutTypes, models, materials );

		Skipper skipper = iso8859::space | kgmGrammar.comment;

		std::string input             = ssKgo.str();
		std::string::const_iterator f = input.cbegin();
		bool res = qi::phrase_parse( f, input.cend(), kgmGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != input.cend() )
		{
			std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "KGO parsing trail: " << std::string( f, end ) << std::endl;
			return nullptr;
		}


		RenderableObject *ro = renderableObjectFromParseData( vertices, indices, vertLayoutTypes, models, materials );
		if ( ro )
			ro->setOriginalFilename( kgoFile );

		return ro;
	}

	RenderableObject* KGObjectParser::renderableObjectFromParseData( std::vector<float> vertices, std::vector<UINT> &indices,
															   std::vector<VertexInputLayout::Type> &vertLayoutTypes,
															   std::vector<KgModelData> &models,
															   std::map<std::string, KgMatData> &materials )
	{
		HRESULT buffCreated = E_FAIL;
		VertexInputLayout vertLayout( vertLayoutTypes );
		ResourceManager::MeshBufferID buffID = ResourceManager::getInst()->addMeshBuffer( vertices, indices, vertLayout, buffCreated );

		if ( FAILED( buffCreated ) )
			return nullptr;

		// create map of materials to meshes to sort meshes per material
		typedef std::pair< std::string, std::vector<RenderableObject::Mesh> > MaterialMeshesPair;
		std::map< std::string, std::vector<RenderableObject::Mesh> > matName2Mesh;
		std::vector<KgModelData>::iterator modelIt;
		for ( modelIt = models.begin(); modelIt != models.end(); ++modelIt )
		{
			RenderableObject::Mesh mesh( modelIt->modelName, modelIt->startIndex, modelIt->indexCount );

			std::map< std::string, std::vector<RenderableObject::Mesh> >::iterator matMeshIt = matName2Mesh.find( modelIt->matName );
			if ( matMeshIt != matName2Mesh.end() )
				matMeshIt->second.push_back( mesh );
			else matName2Mesh.insert( MaterialMeshesPair( modelIt->matName, { mesh } ) );
		}

		//TODO: maybe combine meshes?
		// create object containers
		std::vector<RenderableObject::MaterialMeshContainer> matMeshContainers;
		std::map< std::string, std::vector<RenderableObject::Mesh> >::iterator matMeshIt;
		for ( matMeshIt = matName2Mesh.begin(); matMeshIt != matName2Mesh.end(); ++matMeshIt )
		{
			// sort by mesh
			std::sort( matMeshIt->second.begin(), matMeshIt->second.end() );

			std::map<std::string, KgMatData>::iterator matIt = materials.find( matMeshIt->first );
			if ( matIt != materials.end() )
			{
				std::vector<ID3D11ShaderResourceView*> textures;

				// attempt to load textures
				if ( !matIt->second.diffuseMap.empty() )
				{
					Texture *diffuseMap = TextureManager::getInst()->loadTexture(matIt->second.diffuseMap);
					textures.push_back( diffuseMap->getResourceView() );
				}
				if ( !matIt->second.specularMap.empty() )
				{
					Texture *specularMap = TextureManager::getInst()->loadTexture(matIt->second.specularMap);
					textures.push_back( specularMap->getResourceView() );
				}
				if ( !matIt->second.normalMap.empty() )
				{
					Texture *normalMap = TextureManager::getInst()->loadTexture(matIt->second.normalMap);
					textures.push_back( normalMap->getResourceView() );
				}

				RenderableObject::Material mat( matIt->second.diffuse, matIt->second.specular, textures );
				matMeshContainers.push_back( RenderableObject::MaterialMeshContainer(matMeshIt->second, mat) );
			}
		}

		MeshBuffer meshBuff = ResourceManager::getInst()->getBuffer( buffID );
		return new RenderableObject( KGXCore::getInst()->getDxDevicePtr(), meshBuff, matMeshContainers, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}
}

#pragma warning( default : 4244 )	// restore default warning level
