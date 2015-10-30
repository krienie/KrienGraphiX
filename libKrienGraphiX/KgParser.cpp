
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#pragma warning( disable : 4244 )		// warning C4244: conversion from 'unsigned int' to 'float', possible loss of data. This is intentional.

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_iso8859_1.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/filesystem.hpp>

#include "parser_defines.h"
#include "KGXCore.h"
#include "RenderableObject.h"
#include "VertexInputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ResourceManager.h"
#include "TextureManager.h"

#include "KgParser.h"


BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderVar,
	(kgx::ShaderProgram::ShaderAutoBindType, autoBindType)
	(std::string, type)
	(std::string, name)
	(std::string, defaultValue)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderDef,
	(std::string, name)
	(std::vector<kgx::KgMatData::ShaderVar>, variables)
	(std::vector<std::string>, textures)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(kgx::KgMatData::ShaderDef, vertexShader)
	(kgx::KgMatData::ShaderDef, pixelShader)
);


namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
using namespace qi;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
	RenderableObject* KgParser::loadKGO( const std::string &kgoFile )
	{
		if ( kgoFile.size() == 0 )
		{
			std::cout << "Error (KgParser::loadKGO): Model source file not specified." << std::endl;
			return nullptr;
		}

		if ( !boost::filesystem::exists( kgoFile ) )
		{
			std::cout << "Error (KgParser::loadKGO): Model source file does not exist." << std::endl;
			return nullptr;
		}

		std::stringstream ssKgo;
		std::filebuf fb;
		if ( fb.open( kgoFile, std::ios::in ) )
		{
			ssKgo << &fb;
			fb.close();
		}

		std::vector<float> vertices;
		std::vector<UINT> indices;
		std::vector<VertexInputLayout::Type> vertLayoutTypes;
		std::vector<KgModelData> models;
		std::map<std::string, KgMatData> ShaderPrograms;

		struct KgoGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			KgoGrammar( std::vector<float> &v, std::vector<UINT> &i, std::vector<VertexInputLayout::Type> &l,
						std::vector<KgModelData> &m, std::map<std::string, KgMatData> &mats )
				: KgoGrammar::base_type( start )
			{
				using namespace qi;

				comment = "//" >> skip( blank )[*print];

				vertexInputLayout = qi::string("Position")[_val = VertexInputLayout::Position]
									| qi::string( "TextureCoordinate" )[_val = VertexInputLayout::TextureCoordinate]
									| qi::string( "Normal" )[_val = VertexInputLayout::Normal]
									| qi::string( "Tangent" )[_val = VertexInputLayout::Tangent];
				vertices = "Vertices(" >> vertexInputLayout[phx::push_back( phx::ref(l), qi::_1 )] % char_(',') >> ")"
					>> lit("{") >> *float_[phx::push_back( phx::ref(v), qi::_1 )] >> lit(";") >> "}";

				indices = "Indices()" >> lit("{") >> *uint_[phx::push_back( phx::ref(i), qi::_1 )] >> lit(";") >> "}";

				//TODO: add support for different order of model contents
				models = "Model(" >> *~qi::char_(')') >> lit(")") >> lit("{")
					>> "Indices(" >> int_ >> "," >> int_ >> ");"
					>> "Material(" >> *~qi::char_(')') >> lit(");")
					>> lit("}");

				shaderAutoBindType = qi::string( "CameraProjectionMatrix" )[_val = ShaderProgram::ShaderAutoBindType::CameraProjectionMatrix]
					| qi::string( "CameraViewMatrix" )[_val = ShaderProgram::ShaderAutoBindType::CameraViewMatrix]
					| qi::string( "CameraPosition" )[_val = ShaderProgram::ShaderAutoBindType::CameraPosition]
					| qi::string( "CameraTarget" )[_val = ShaderProgram::ShaderAutoBindType::CameraTarget]
					| qi::string( "CameraFieldOfView" )[_val = ShaderProgram::ShaderAutoBindType::CameraFieldOfView]
					| qi::string( "CameraAspectRatio" )[_val = ShaderProgram::ShaderAutoBindType::CameraAspectRatio]
					| qi::string( "CameraNearZ" )[_val = ShaderProgram::ShaderAutoBindType::CameraNearZ]
					| qi::string( "CameraFarZ" )[_val = ShaderProgram::ShaderAutoBindType::CameraFarZ]
					| qi::string( "ObjectModelMatrix" )[_val = ShaderProgram::ShaderAutoBindType::ObjectModelMatrix]
					| qi::string( "ObjectNormalMatrix" )[_val = ShaderProgram::ShaderAutoBindType::ObjectNormalMatrix]
					| eps[_val = ShaderProgram::ShaderAutoBindType::NoAutoBind];

				shaderVariable = shaderAutoBindType >> lexeme[*(print - iso8859::space)] >> *~qi::char_('(') >> lit('(') >> *~qi::char_(')') >> lit(");");
				texture = "Texture(" >> *~qi::char_(')') >> lit(");");
				shaderDefinition = (qi::lit("VertexShader") | qi::lit("PixelShader")) >> lit("(") >> *~qi::char_(')') >> lit(")")
					>> lit("{") >> *shaderVariable >> *texture >> lit("}");

				ShaderProgram = shaderDefinition >> shaderDefinition;
				nameMatPair = omit[lit("Material(")] >> *~qi::char_(')') >> omit[lit(")")]
					>> omit[lit("{")] >> ShaderProgram >> omit[lit("}")];

				start = *(comment | vertices | indices | models[phx::push_back( phx::ref(m), qi::_1 )]
						   | nameMatPair[phx::insert( phx::ref(mats), qi::_1 )]);
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, Skipper> vertices, indices;
			qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
			qi::rule<std::string::const_iterator, std::string(), Skipper> vertexInputLayoutName;

			qi::rule<std::string::const_iterator, KgModelData(), Skipper> models;

			qi::rule<std::string::const_iterator, ShaderProgram::ShaderAutoBindType()> shaderAutoBindType;
			qi::rule<std::string::const_iterator, KgMatData::ShaderVar(), Skipper> shaderVariable;
			qi::rule<std::string::const_iterator, std::string(), Skipper> texture;
			qi::rule<std::string::const_iterator, KgMatData::ShaderDef(), Skipper> shaderDefinition;

			qi::rule<std::string::const_iterator, KgMatData(), Skipper> ShaderProgram;
			qi::rule<std::string::const_iterator, std::pair<std::string, KgMatData>(), Skipper> nameMatPair;

			qi::rule<std::string::const_iterator> comment;
		} kgmGrammar( vertices, indices, vertLayoutTypes, models, ShaderPrograms );

		Skipper skipper = iso8859::space;

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

		return renderableObjectFromParseData( vertices, indices, vertLayoutTypes, models, ShaderPrograms );
	}

	RenderableObject* KgParser::renderableObjectFromParseData( std::vector<float> vertices, std::vector<UINT> &indices,
															   std::vector<VertexInputLayout::Type> &vertLayoutTypes,
															   std::vector<KgModelData> &models,
															   std::map<std::string, KgMatData> &ShaderPrograms )
	{
		HRESULT buffCreated = E_FAIL;
		VertexInputLayout vertLayout( vertLayoutTypes );
		ResourceManager::MeshBufferID buffID = ResourceManager::getInst()->addMeshBuffer( vertices, indices, vertLayout, buffCreated );

		if ( FAILED( buffCreated ) )
			return nullptr;


		// create ShaderPrograms
		std::map<std::string, ShaderProgram*> shaderProgramPtrMap;
		std::map<std::string, KgMatData>::iterator matIt;
		for ( matIt = ShaderPrograms.begin(); matIt != ShaderPrograms.end(); ++matIt )
		{
			ShaderProgram *ShaderProgram = ResourceManager::getInst()->createShaderProgram();

			VertexShader *vertShader = ShaderProgram->createVertexShader( matIt->second.vertexShader.name, vertLayout );
			setShaderVariables( ShaderProgram, vertShader, matIt->second.vertexShader );
			// add vertex shader textures
			std::vector<std::string>::iterator it;
			for ( it = matIt->second.vertexShader.textures.begin(); it != matIt->second.vertexShader.textures.end(); ++it )
			{
				Texture *tex = TextureManager::getInst()->loadTexture( *it );
				if ( tex )
					vertShader->addTexture( tex );
			}

			//TODO: add support for other shader types

			PixelShader *pixShader = ShaderProgram->createPixelShader( matIt->second.pixelShader.name );
			setShaderVariables( ShaderProgram, pixShader, matIt->second.pixelShader );
			// add pixel shader textures
			for ( it = matIt->second.pixelShader.textures.begin(); it != matIt->second.pixelShader.textures.end(); ++it )
			{
				Texture *tex = TextureManager::getInst()->loadTexture( *it );
				if ( tex )
					pixShader->addTexture( tex );
			}


			shaderProgramPtrMap.insert( std::pair<std::string, kgx::ShaderProgram*>( matIt->first, ShaderProgram ) );
		}


		// create models
		typedef std::pair< std::string, std::vector<RenderableObject::Mesh> > ShaderProgramMeshPair;
		std::map< std::string, std::vector<RenderableObject::Mesh> > ShaderProgramMeshPtrMap;
		std::vector<KgModelData>::iterator modIt;
		for ( modIt = models.begin(); modIt != models.end(); ++modIt )
		{
			RenderableObject::Mesh mesh( modIt->modelName, modIt->startIndex, modIt->indexCount );

			std::map< std::string, std::vector<RenderableObject::Mesh> >::iterator matMeshIt = ShaderProgramMeshPtrMap.find( modIt->matName );
			if ( matMeshIt != ShaderProgramMeshPtrMap.end() )
				matMeshIt->second.push_back( mesh );
			else ShaderProgramMeshPtrMap.insert( ShaderProgramMeshPair( modIt->matName, { mesh } ) );
		}

		// create meshes
		// sort meshes by ShaderProgram
		//		sort meshes by start index => maybe combine meshes?

		//TODO: maybe combine meshes?
		// create object containers
		std::vector<RenderableObject::ObjectContainer> objContainers;
		std::map< std::string, std::vector<RenderableObject::Mesh> >::iterator matMeshIt;
		for ( matMeshIt = ShaderProgramMeshPtrMap.begin(); matMeshIt != ShaderProgramMeshPtrMap.end(); ++matMeshIt )
		{
			// sort by mesh
			std::sort( matMeshIt->second.begin(), matMeshIt->second.end() );

			// assumes the ShaderProgram for the mesh is always found
			objContainers.push_back( RenderableObject::ObjectContainer( matMeshIt->second, shaderProgramPtrMap.find(matMeshIt->first)->second ) );
		}
			
		MeshBuffer meshBuff = ResourceManager::getInst()->getBuffer( buffID );

		return new RenderableObject( KGXCore::getInst()->getDxDevicePtr(), meshBuff, objContainers, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	void KgParser::setShaderVariables( ShaderProgram *ShaderProgram, ShaderBase *sh, const KgMatData::ShaderDef &shDef )
	{
		std::vector<KgMatData::ShaderVar>::const_iterator it;
		for ( it = shDef.variables.begin(); it != shDef.variables.end(); ++it )
		{
			if ( it->autoBindType != ShaderProgram::ShaderAutoBindType::NoAutoBind )
				ShaderProgram->addAutoShaderVar( sh, it->name, it->autoBindType );
			//TODO: add support for default constant values
			//else sh->updateConstantVariable( it->name, nullptr );
		}
	}
}

#pragma warning( default : 4244 )	// restore default warning level
