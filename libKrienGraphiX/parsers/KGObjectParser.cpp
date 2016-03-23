
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
#include "../IOManager.h"
#include "../ResourceManager.h"

#include "KGObjectParser.h"


BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT3,
	(float, x)
	(float, y)
	(float, z)
	);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(DirectX::XMFLOAT3, diffuse)
	(DirectX::XMFLOAT3, specular)
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
		if ( !boost::filesystem::path( kgoFile ).is_absolute() )
			absKGOFile = IOManager::getInst()->getAbsolutePath( kgoFile );
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
				using namespace qi;

				comment = "//" >> skip( blank )[*print];

				vertexInputLayout = qi::string("Position")[_val = VertexInputLayout::Position]
									| qi::string("TextureCoordinate")[_val = VertexInputLayout::TextureCoordinate]
									| qi::string("Normal")[_val = VertexInputLayout::Normal]
									| qi::string("Tangent")[_val = VertexInputLayout::Tangent];
				vertices = "Vertices" >> lit("(") >> vertexInputLayout[phx::push_back( phx::ref(l), qi::_1 )] % char_(',') >> ")"
					>> lit("{") >> *float_[phx::push_back( phx::ref(v), qi::_1 )] >> "}";

				indices = "Indices" >> lit("(") >> lit(")") >> lit("{") >> *uint_[phx::push_back( phx::ref(i), qi::_1 )] >> "}";

				//TODO: add support for different order of model contents
				models = "Model" >> lit("(") >> *~qi::char_(')') >> lit(")") >> lit("{")
					>> "Indices" >> lit("(") >> int_ >> lit(",") >> int_ >> lit(")")
					>> "Material" >> lit("(") >> *~qi::char_(')') >> lit(")")
					>> lit("}");

				float3Property = lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");
				material =    lit("float3") >> lit("diffuse") >> float3Property
						   >> lit("float3") >> lit("specular") >> float3Property;
				nameMatPair = lit("Material") >> lit("(") >> *~qi::char_(')') >> lit(")")
					>> lit( "{" ) >> material >> lit( "}" );

				start = *(vertices | indices | models[phx::push_back( phx::ref(m), qi::_1 )]
						   | nameMatPair[phx::insert( phx::ref(mats), qi::_1 )]);
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, Skipper> vertices, indices;
			qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
			qi::rule<std::string::const_iterator, std::string(), Skipper> vertexInputLayoutName;

			qi::rule<std::string::const_iterator, KgModelData(), Skipper> models;

			qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> float3Property;
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


		//TODO: set material properties

		// create models
		std::vector<RenderableObject::Mesh> meshes;
		std::vector<KgModelData>::iterator modIt;
		for ( modIt = models.begin(); modIt != models.end(); ++modIt )
		{
			//TODO: maybe combine meshes? => if so, do it in KGObjectGenerator, not in the parser
			meshes.push_back( RenderableObject::Mesh(modIt->modelName, modIt->startIndex, modIt->indexCount) );

			//TODO: do something with the materials...
		}
			
		MeshBuffer meshBuff = ResourceManager::getInst()->getBuffer( buffID );

		return new RenderableObject( KGXCore::getInst()->getDxDevicePtr(), meshBuff, meshes, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}
}

#pragma warning( default : 4244 )	// restore default warning level
