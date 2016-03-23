
#pragma warning( disable : 4244 )		// warning C4244: conversion from 'long' to 'float', possible loss of data. This is intentional.

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <sstream>

#include "../Camera.h"
#include "../Scene.h"
#include "../RenderableObject.h"
#include "../IOManager.h"
#include "../parsers/KGObjectParser.h"
#include "KGObjectGenerator.h"
#include "KGSceneGenerator.h"

BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT3,
	(float, x)
	(float, y)
	(float, z)
);

namespace karma = boost::spirit::karma;
namespace phx   = boost::phoenix;
typedef std::back_insert_iterator<std::string> BackInsertIt;

namespace kgx
{
	//TODO: update KGSceneGenerator
	void KGSceneGenerator::generate( Scene *inputScene, /*RenderCore *renderCore,*/ const std::string &roSavePath, std::string &output )
	{
		if ( !inputScene )
		{
			std::cout << "Error (KGSceneGenerator::generate): inputScene is null. Aborting" << std::endl;
			return;
		}

		// construct header string
		std::stringstream outSS;
		outSS << "// KrienGraphiX Scene v0.1 - (c)2016 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		outSS.imbue( std::locale( outSS.getloc(), facet ) );
		outSS << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		//TODO: for now, do not write the RenderableObject to disk. Use the name as filename (of maybe store the filename in RO...)
		Scene::const_renobjectiterator renObjIt;
		for ( renObjIt = inputScene->getRenObjectCBegin(); renObjIt != inputScene->getRenObjectCEnd(); ++renObjIt )
		{
			std::string roString;
			if ( generateRenderableObject(*renObjIt, roString) )
				outSS << roString << std::endl;
		}

		Scene::const_cameraiterator camIt;
		for ( camIt = inputScene->getCameraCBegin(); camIt != inputScene->getCameraCEnd(); ++camIt )
		{
			std::string camString;
			if ( generateCamera(camIt->second, camString) )
				outSS << camString << std::endl;
		}

		std::string ambientString;
		if ( generateAmbientLight( inputScene->getAmbient(), ambientString ) )
			outSS << ambientString << std::endl;

		Scene::const_lightiterator lightIt;
		for ( lightIt = inputScene->getLightCBegin(); lightIt != inputScene->getLightCEnd(); ++lightIt )
		{
			std::string lightString;
			if ( generateLight(*lightIt, lightString) )
				outSS << lightString << std::endl;
		}

		output = outSS.str();
	}


	bool KGSceneGenerator::generateRenderableObject( RenderableObject *ro, std::string &output )
	{
		struct RoGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			RoGram( const std::string &name, const std::string &filename, DirectX::XMFLOAT3 &pos, const DirectX::XMFLOAT3 &scale )
				: RoGram::base_type( output )
			{
				using namespace karma;

				objFile = "\tsource(" << karma::string(filename) << ")" << no_delimit[eol];

				objPos = "\tposition(" << no_delimit[karma::double_(pos.x)] << ","
									   << no_delimit[karma::double_(pos.y)] << ","
									   << karma::double_(pos.z) << ")" << no_delimit[eol];

				objScale = "\tscale(" << no_delimit[karma::double_(scale.x)] << ","
									  << no_delimit[karma::double_(scale.y)] << ","
									  << karma::double_(scale.z) << ")" << no_delimit[eol];

				output = "RenderableObject(" << karma::string(name) << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< objFile << objPos << objScale
					<< "}" << no_delimit[eol];
			}
			
			karma::rule<BackInsertIt, karma::space_type> output, objFile,objPos, objScale;

		} roGram( ro->getName(), ro->getOriginalFilename(), ro->getPosition(), ro->getScale() );


		BackInsertIt sink( output );
		bool result = karma::generate_delimited( sink, roGram, karma::space );

		return result;
	}

	bool KGSceneGenerator::generateCamera( Camera *cam, std::string &output )
	{
		struct CamGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			CamGram( const std::string &name, float fov, float aspect, float nearZ, float farZ,
					 const DirectX::XMFLOAT3 &camEye, const DirectX::XMFLOAT3 &camTarget, const DirectX::XMFLOAT3 &camUp )
				: CamGram::base_type( output )
			{
				using namespace karma;

				fieldOfView = "\tfieldOfView(" << karma::double_(fov)    << ")" << no_delimit[eol];
				aspectRatio = "\taspect("      << karma::double_(aspect) << ")" << no_delimit[eol];
				nearPlane   = "\tnearZ("       << karma::double_(nearZ)  << ")" << no_delimit[eol];
				farPlane    = "\tfarZ("        << karma::double_(farZ)   << ")" << no_delimit[eol];
				
				eye    = "\teye(" << no_delimit[karma::double_(camEye.x)] << ","
							      << no_delimit[karma::double_(camEye.y)] << ","
							      << karma::double_(camEye.z) << ")" << no_delimit[eol];
				target = "\ttarget(" << no_delimit[karma::double_(camTarget.x)] << ","
							         << no_delimit[karma::double_(camTarget.y)] << ","
							         << karma::double_(camTarget.z) << ")" << no_delimit[eol];
				up     = "\tup(" << no_delimit[karma::double_(camUp.x)] << ","
								 << no_delimit[karma::double_(camUp.y)] << ","
								 << karma::double_(camUp.z) << ")" << no_delimit[eol];

				output = "Camera(" << karma::string(name) << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< fieldOfView << aspectRatio << nearPlane << farPlane << eye << target << up
					<< "}" << no_delimit[eol];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt, karma::space_type> fieldOfView, aspectRatio, nearPlane, farPlane, eye, target, up;

		} camGram( cam->getName(), cam->getFOV(), cam->getAspectRatio(), cam->getNearZ(), cam->getFarZ(),
				   cam->getPosition(), cam->getTarget(), cam->getUp() );


		BackInsertIt sink( output );
		bool result = karma::generate_delimited( sink, camGram, karma::space );
		return result;
	}

	bool KGSceneGenerator::generateAmbientLight( const DirectX::XMFLOAT3 &ambColor, std::string &output )
	{
		struct AmbientGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			AmbientGram( const DirectX::XMFLOAT3 &ambient ) : AmbientGram::base_type( output )
			{
				using namespace karma;

				output = "Light()" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\ttype(AMBIENT)" << no_delimit[eol]
					<< "\tcolor(" << no_delimit[karma::double_(ambient.x)] << ","
								  << no_delimit[karma::double_(ambient.y)] << ","
								  << karma::double_(ambient.z) << ")" << no_delimit[eol]
					<< "}" << no_delimit[eol];
			}

			karma::rule<BackInsertIt, karma::space_type> output;

		} ambientGram( ambColor );


		BackInsertIt sink( output );
		bool result = karma::generate_delimited( sink, ambientGram, karma::space );
		return result;
	}

	bool KGSceneGenerator::generateLight( const Light &li, std::string &output )
	{
		// for now only directional lights are supported

		struct LightGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			LightGram( const DirectX::XMFLOAT3 &direction, float intensity ) : LightGram::base_type( output )
			{
				using namespace karma;

				output = "Light()" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\ttype(DIRECTIONAL)" << no_delimit[eol]
					<< "\tdirection(" << no_delimit[karma::double_( direction.x )] << ","
								      << no_delimit[karma::double_(direction.y)] << ","
								      << karma::double_(direction.z) << ")" << no_delimit[eol]
					<< "\tintensity(" << karma::double_(intensity) << ")" << no_delimit[eol] 
					<< "}" << no_delimit[eol];
			}

			karma::rule<BackInsertIt, karma::space_type> output;

		} lightGram( li.direction, li.intensity );


		BackInsertIt sink( output );
		bool result = karma::generate_delimited( sink, lightGram, karma::space );
		return result;
	}
}

#pragma warning( default : 4244 )	// restore default warning level
