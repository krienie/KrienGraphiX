
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
#include "../Scene.h"
#include "../VertexInputLayout.h"
#include "../Filesystem.h"
#include "../ResourceManager.h"
#include "../Texture.h"

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

namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
	void KGObjectParser::loadKGO( const std::string &kgoFile, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale, Scene *scene )
	{
		std::string kgObjectData;
		if ( !filesystem::openFile(kgoFile, kgObjectData) )
		{
			std::cout << "(KGSceneParser::loadKGO): Error loading kgobject file." << std::endl;
			return;
		}

		std::vector<float> vertices;
		std::vector<UINT> indices;
		std::vector<VertexInputLayout::Type> vertLayoutTypes;
		std::vector<KgModelData> models;
		struct KgoGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			KgoGrammar( std::vector<float> &v, std::vector<UINT> &i, std::vector<VertexInputLayout::Type> &l,
						std::vector<KgModelData> &m )
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

				start = *(vertices | indices | models[phx::push_back( phx::ref(m), qi::_1 )]);
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, Skipper> vertices, indices;
			qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
			qi::rule<std::string::const_iterator, std::string(), Skipper> vertexInputLayoutName;
			qi::rule<std::string::const_iterator, KgModelData(), Skipper> models;

			qi::rule<std::string::const_iterator> comment;
		} kgmGrammar( vertices, indices, vertLayoutTypes, models );

		Skipper skipper = iso8859::space | kgmGrammar.comment;

		std::string::const_iterator f = kgObjectData.cbegin();
		bool res = qi::phrase_parse( f, kgObjectData.cend(), kgmGrammar, skipper );
		//TODO: handle res == false

		// print everything that hasn't been processed by the parser
		if ( f != kgObjectData.cend() )
		{
			std::string::const_iterator end = std::distance( f, kgObjectData.cend() ) > 100 ? f + 100 : kgObjectData.cend();
			std::cout << std::endl << "KGO parsing trail: " << std::string( f, end ) << std::endl;
			return;
		}


		res = addParsedDataToScene( vertices, indices, vertLayoutTypes, models, position, scale, scene );

		//TODO: handle res == false
	}

	bool KGObjectParser::addParsedDataToScene( std::vector<float> vertices, std::vector<UINT> &indices,
												std::vector<VertexInputLayout::Type> &vertLayoutTypes,
												std::vector<KgModelData> &models,
												const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &scale,
												Scene *scene )
	{
		HRESULT buffCreated = E_FAIL;
		VertexInputLayout vertLayout( vertLayoutTypes );
		MeshBufferID meshBuffer = ResourceManager::getInst()->addMeshBuffer( vertices, indices, vertLayout, buffCreated );

		if ( FAILED( buffCreated ) )
			return false;


		std::vector<KgModelData>::const_iterator it;
		for ( it = models.cbegin(); it != models.cend(); ++it )
		{
			RenderableObject ro;
			ro.topology   = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			ro.meshBuffer = meshBuffer;

			ro.indexCount = it->indexCount;
			ro.startIndex = it->startIndex;
			ro.baseVertex = 0u;

			ro.material      = ResourceManager::getInst()->getMaterial( it->matName );
			ro.shaderProgram = ResourceManager::getInst()->getDefaultShaderProgram();

			ro.xPos   = position.x;
			ro.yPos   = position.y;
			ro.zPos   = position.z;
			ro.xScale = scale.x;
			ro.yScale = scale.y;
			ro.zScale = scale.z;

			scene->addRenderableObject( ro );
		}

		return true;
	}
}

#pragma warning( default : 4244 )	// restore default warning level
