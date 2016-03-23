
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_iso8859_1.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/filesystem.hpp>

#include "../Camera.h"
#include "../IOManager.h"
#include "../ResourceManager.h"
#include "../RenderableObject.h"
#include "../RenderCore.h"
#include "../RenderWindow.h"
#include "../Scene.h"
#include "../Texture.h"
#include "../TextureManager.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "KGObjectParser.h"
#include "KGSceneParser.h"

BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT3,
	(float, x)
	(float, y)
	(float, z)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgShaderProgramData::ShaderVar,
	(kgx::ShaderProgram::ShaderAutoBindType, autoBindType)
	(std::string, type)
	(std::string, name)
	(std::string, defaultValue)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgShaderProgramData::ShaderDef,
	(std::string, filename)
	(std::vector<kgx::KgShaderProgramData::ShaderVar>, variables)
	(std::vector<std::string>, textures)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgShaderProgramData,
	(kgx::KgShaderProgramData::ShaderDef, vertexShader)
	(kgx::KgShaderProgramData::ShaderDef, pixelShader)
);

namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgx
{
	Scene* KGSceneParser::loadKGScene( const std::string &kgsceneFile, RenderWindow *renderWin )
	{
		std::string absSceneFile = kgsceneFile;
		if ( !boost::filesystem::path(kgsceneFile).is_absolute() )
			absSceneFile = IOManager::getInst()->getAbsolutePath( kgsceneFile );
		if ( absSceneFile.size() == 0 )
		{
			std::cout << "Error (KGSceneParser::loadKGScene): Scene source file not specified." << std::endl;
			return nullptr;
		}

		if ( !boost::filesystem::exists( absSceneFile ) )
		{
			std::cout << "Error (KGSceneParser::loadKGScene): Scene source file does not exist." << std::endl;
			return nullptr;
		}

		std::stringstream ssKgs;
		std::filebuf fb;
		if ( fb.open( absSceneFile, std::ios::in ) )
		{
			ssKgs << &fb;
			fb.close();
		}


		std::vector<std::string> objects;
		std::vector<std::string> cameras;
		std::vector<std::string> lights;
		std::vector<std::string> renderPasses;
		struct KgsceneGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			KgsceneGrammar( std::vector<std::string> &renderObjects, std::vector<std::string> &cams,
							std::vector<std::string> &lights, std::vector<std::string> &renPasses )
				: KgsceneGrammar::base_type( start )
			{
				using namespace qi;

				comment = "//" >> skip(blank)[*print];

				renderableObject = "RenderableObject" >> *~qi::char_('}') >> char_('}');
				camera           = "Camera" >> *~qi::char_('}') >> char_('}');
				light            = "Light" >> *~qi::char_('}') >> char_('}');

				shader = (qi::string("VertexShader") | qi::string("PixelShader")) >> lexeme[ *~qi::char_('}') ] >> char_('}');
				renderPass = qi::string("RenderPass") >> char_('(') >> *~qi::char_(')') >> char_(')')
								>> qi::char_('{') >> *shader >> char_('}');
				renderCore = "RenderCore" >> qi::char_('{')
								>> *renderPass[phx::push_back( phx::ref(renPasses), qi::_1 )]
								>> qi::char_('}');


				start = *(comment | renderableObject[phx::push_back( phx::ref( renderObjects ), qi::_1 )]
						   | camera[phx::push_back( phx::ref( cams ), qi::_1 )]
						   | light[phx::push_back( phx::ref( lights ), qi::_1 )]
						   | renderCore);
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, std::string(), Skipper> renderableObject;
			qi::rule<std::string::const_iterator, std::string(), Skipper> camera;
			qi::rule<std::string::const_iterator, std::string(), Skipper> light;
			qi::rule<std::string::const_iterator, std::string(), Skipper> shader;
			qi::rule<std::string::const_iterator, std::string(), Skipper> renderPass;
			qi::rule<std::string::const_iterator, Skipper> renderCore;
			qi::rule<std::string::const_iterator> comment;
		} kgsGrammar( objects, cameras, lights, renderPasses );

		Skipper skipper = iso8859::space | kgsGrammar.comment;

		std::string input = ssKgs.str();
		std::string::const_iterator f = input.cbegin();
		bool res = qi::phrase_parse( f, input.cend(), kgsGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != input.cend() )
		{
			std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "KGScene parsing trail: " << std::string( f, end ) << std::endl;
			return nullptr;
		}

		Scene *newScene = new Scene();

		// parse RenderableObjects
		std::vector<std::string>::const_iterator it;
		for ( it = objects.cbegin(); it != objects.cend(); ++it )
		{
			RenderableObject *rObj = loadRenderObject( *it );
			if ( rObj )
				newScene->claimRenderableObject( rObj );
			else std::cout << "Warning (KGSceneParser::loadKGScene): Error loading RenderableObject. Skipping." << std::endl;
		}

		// parse Camera's
		for ( it = cameras.cbegin(); it != cameras.cend(); ++it )
			if ( !loadCamera(*it, renderWin, newScene) )
				std::cout << "Warning (KGSceneParser::loadKGScene): Error creating camera. Skipping." << std::endl;

		// parse Lights
		for ( it = lights.cbegin(); it != lights.cend(); ++it )
			if ( !loadLight(*it, newScene) )
				std::cout << "Warning (KGSceneParser::loadKGScene): Error creating light. Skipping." << std::endl;

		// parse RenderPasses
		for ( it = renderPasses.cbegin(); it != renderPasses.cend(); ++it )
			if ( !loadRenderPass( *it, renderWin->getRenderCorePtr() ) )
				std::cout << "Warning (KGSceneParser::loadKGScene): Error creating RenderPass. Skipping." << std::endl;

		return newScene;
	}


	RenderableObject* KGSceneParser::loadRenderObject( const std::string &objString )
	{
		std::string name;
		std::string sourceFile;
		DirectX::XMFLOAT3 position(0.0f, 0.0f, 0.0f);
		float scale = 1.0f;
		//TODO: add rotation
		struct RenderObjectGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			RenderObjectGrammar( std::string &name, std::string &source, DirectX::XMFLOAT3 &pos, float &scale )
				: RenderObjectGrammar::base_type( start )
			{
				using namespace qi;

				objName  = lit("(") >> *~qi::char_(')') >> lit(")");
				file     = "source" >> lit("(") >> *~qi::char_('.') >> qi::string(".kgo") >> lit(")");
				position = "position" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");
				objScale = "scale" >> lit("(") >> qi::float_ >> lit(")");

				start = objName[phx::ref(name) = qi::_1]
					>> lit("{")
					>> *(file[phx::ref(source) = qi::_1] | position[phx::ref(pos) = qi::_1] | objScale[phx::ref(scale) = qi::_1])
					>> lit("}");
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, std::string(), Skipper> objName, file;
			qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> position;
			qi::rule<std::string::const_iterator, float(), Skipper> objScale;
		} kgsGrammar( name, sourceFile, position, scale );

		Skipper skipper = iso8859::space;

		std::string::const_iterator f = objString.cbegin();
		bool res = qi::phrase_parse( f, objString.cend(), kgsGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != objString.cend() )
		{
			std::string::const_iterator end = std::distance( f, objString.cend() ) > 100 ? f + 100 : objString.cend();
			std::cout << std::endl << "RenderableObject parsing trail: " << std::string( f, end ) << std::endl;
			return nullptr;
		}

		RenderableObject *renObj = kgx::KGObjectParser::loadKGO(sourceFile);
		if ( !renObj )
			return nullptr;

		// apply settings
		renObj->setName( name );
		renObj->setPosition( position.x, position.y, position.z );
		renObj->setScale( scale );

		return renObj;
	}

	bool KGSceneParser::loadCamera( const std::string &camString, RenderWindow *renderWin, Scene* parentScene )
	{
		if ( !parentScene )
			return false;

		std::string name;
		float fov    = DirectX::XM_PIDIV4;
		float aspect = renderWin ? renderWin->getAspectRatio() : 1.0f;
		float nearZ  = 0.01f;
		float farZ   = 1000.0f;
		DirectX::XMFLOAT3 eye( 0.0f, 0.0f, 0.0f );
		DirectX::XMFLOAT3 target( 0.0f, 0.0f, -1.0f );
		DirectX::XMFLOAT3 up( 0.0f, 1.0f, 0.0f );
		struct CameraGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			CameraGrammar( std::string &name, float &fov, float &aspect, float &nearZ, float &farZ,
						   DirectX::XMFLOAT3 &eye, DirectX::XMFLOAT3 &target, DirectX::XMFLOAT3 &up )
				: CameraGrammar::base_type( start )
			{
				using namespace qi;

				objName = lit("(") >> *~qi::char_(')') >> lit(")");
				fieldOfView = "fieldOfView"
					>> (qi::string("(PI)")[_val = DirectX::XM_PI]
					| qi::string("(2PI)")[_val = DirectX::XM_2PI]
					| qi::string("(1DIVPI)")[_val = DirectX::XM_1DIVPI]
					| qi::string("(1DIV2PI)")[_val = DirectX::XM_1DIVPI]
					| qi::string("(PIDIV2)")[_val = DirectX::XM_PIDIV2]
					| qi::string("(PIDIV4)")[_val = DirectX::XM_PIDIV4]
					| qi::float_);
				aspectRatio = "aspect" >> lit("(") >> qi::float_ >> lit(")");
				nearPlane   = "nearZ" >> lit("(") >> qi::float_ >> lit(")");
				farPlane    = "farZ" >> lit("(") >> qi::float_ >> lit(")");
				eyePos      = "eye" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");
				targetPos   = "target" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");
				upVec       = "up" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");

				start = objName[phx::ref(name) = qi::_1]
					>> lit("{")
					>> *( fieldOfView[phx::ref(fov) = qi::_1]     | aspectRatio[phx::ref(aspect) = qi::_1]
							| nearPlane[phx::ref(nearZ) = qi::_1] | farPlane[phx::ref(farZ) = qi::_1]
							| eyePos[phx::ref(eye) = qi::_1]      | targetPos[phx::ref(target) = qi::_1]
							| upVec[phx::ref(up) = qi::_1] )
					>> lit("}");
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, std::string(), Skipper> objName;
			qi::rule<std::string::const_iterator, float(), Skipper> fieldOfView, aspectRatio, nearPlane, farPlane;
			qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> eyePos, targetPos, upVec;
		} camGrammar( name, fov, aspect, nearZ, farZ, eye, target, up );

		Skipper skipper = iso8859::space;

		std::string::const_iterator f = camString.cbegin();
		bool res = qi::phrase_parse( f, camString.cend(), camGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != camString.cend() )
		{
			std::string::const_iterator end = std::distance( f, camString.cend() ) > 100 ? f + 100 : camString.cend();
			std::cout << std::endl << "Camera parsing trail: " << std::string( f, end ) << std::endl;
			return false;
		}

		// apply settings
		Scene::CameraID camID = parentScene->createCamera( fov, aspect, nearZ, farZ, eye, target, up );
		Camera *newCam = parentScene->getCamera( camID );
		newCam->setName( name );

		return true;
	}

	bool KGSceneParser::loadLight( const std::string &lightString, Scene *parentScene )
	{
		if ( !parentScene )
			return false;

		std::string name;
		bool isAmbient  = false;
		float intensity = 1.0f;
		DirectX::XMFLOAT3 color( 1.0f, 1.0f, 1.0f );
		DirectX::XMFLOAT3 direction( 1.0f, 1.0f, 1.0f );
		struct LightGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			LightGrammar( std::string &name, bool &isAmb, float &intens, DirectX::XMFLOAT3 &clr, DirectX::XMFLOAT3 &dir )
						   : LightGrammar::base_type( start )
			{
				using namespace qi;

				objName   = lit("(") >> *~qi::char_(')') >> lit(")");
				type      = "type" >> (qi::string("(AMBIENT)")[_val = true]| qi::string("(DIRECTIONAL)")[_val = false]);
				intensity = "intensity" >> lit("(") >> qi::float_ >> lit(")");
				color     = "color" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");
				direction = "direction" >> lit("(") >> qi::float_ >> lit(",") >> qi::float_ >> lit(",") >> qi::float_ >> lit(")");

				start = objName[phx::ref(name) = qi::_1]
					>> lit("{")
					>> *( type[phx::ref(isAmb) = qi::_1]    | intensity[phx::ref(intens) = qi::_1]
							| color[phx::ref(clr) = qi::_1] | direction[phx::ref(dir) = qi::_1] )
					>> lit("}");
			}

			qi::rule<std::string::const_iterator, Skipper> start;
			qi::rule<std::string::const_iterator, std::string(), Skipper> objName;
			qi::rule<std::string::const_iterator, bool(), Skipper> type;
			qi::rule<std::string::const_iterator, float(), Skipper> intensity;
			qi::rule<std::string::const_iterator, DirectX::XMFLOAT3(), Skipper> color, direction;
		} lGrammar( name, isAmbient, intensity, color, direction );

		Skipper skipper = iso8859::space;

		std::string::const_iterator f = lightString.cbegin();
		bool res = qi::phrase_parse( f, lightString.cend(), lGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != lightString.cend() )
		{
			std::string::const_iterator end = std::distance( f, lightString.cend() ) > 100 ? f + 100 : lightString.cend();
			std::cout << std::endl << "Light parsing trail: " << std::string( f, end ) << std::endl;
			return false;
		}

		// apply settings
		if ( !isAmbient )
			parentScene->addDirectionalLight( direction, intensity );
		else parentScene->setAmbient( color );

		return true;
	}

	bool KGSceneParser::loadRenderPass( const std::string &passString, RenderCore *renderCore )
	{
		if ( !renderCore )
			return false;

		std::vector<VertexInputLayout::Type> vertLayoutTypes;
		std::map<int, KgShaderProgramData> shaderPrograms;
		struct PassGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			PassGrammar( std::map<int, KgShaderProgramData> &shaderProgs, std::vector<VertexInputLayout::Type> &l )
				: PassGrammar::base_type( start )
			{
				using namespace qi;

				comment = "//" >> skip( blank )[*print];

				shaderAutoBindType = qi::string("CameraProjectionMatrix")[_val = ShaderProgram::ShaderAutoBindType::CameraProjectionMatrix]
					| qi::string("CameraViewMatrix")[_val = ShaderProgram::ShaderAutoBindType::CameraViewMatrix]
					| qi::string("CameraPosition")[_val = ShaderProgram::ShaderAutoBindType::CameraPosition]
					| qi::string("CameraTarget")[_val = ShaderProgram::ShaderAutoBindType::CameraTarget]
					| qi::string("CameraFieldOfView")[_val = ShaderProgram::ShaderAutoBindType::CameraFieldOfView]
					| qi::string("CameraAspectRatio")[_val = ShaderProgram::ShaderAutoBindType::CameraAspectRatio]
					| qi::string("CameraNearZ")[_val = ShaderProgram::ShaderAutoBindType::CameraNearZ]
					| qi::string("CameraFarZ")[_val = ShaderProgram::ShaderAutoBindType::CameraFarZ]
					| (!qi::string("Texture"))[_val = ShaderProgram::ShaderAutoBindType::NoAutoBind];		// make sure not to eat any possible texture definitions

				shaderVariable = shaderAutoBindType >> lexeme[*(print - iso8859::space)] >> *~qi::char_('(') >> lit("(") >> *~qi::char_(')') >> lit(")");
				texture = qi::string("Texture") >> lit("(") >> *~qi::char_(')') >> lit(")");

				vertexInputLayout = qi::string("Position")[_val = VertexInputLayout::Position]
					| qi::string("TextureCoordinate")[_val = VertexInputLayout::TextureCoordinate]
					| qi::string("Normal")[_val = VertexInputLayout::Normal]
					| qi::string("Tangent")[_val = VertexInputLayout::Tangent];
				vertexShaderDefinition = lit("VertexShader") >> lit("(") >> *~qi::char_(')') >> lit(")")
					>> lit(":") >> omit[ vertexInputLayout[phx::push_back( phx::ref(l), qi::_1 )] % char_(',') ]
					>> lit("{") >> *shaderVariable >> *texture >> lit("}");
				pixelShaderDefinition  = lit("PixelShader") >> lit("(") >> *~qi::char_(')') >> lit(")")
					>> lit("{") >> *shaderVariable >> *texture >> lit("}");
				//TODO: allow different order of shaderVariable and textures

				//TODO: make shaderProgram parsing more robust => allow different orders or programs and allow some programs to be missing (no hull/domain shaders, for example)
				shaderProgram = vertexShaderDefinition >> pixelShaderDefinition;
				intMatPair = lit("RenderPass") >> lit("(") >> qi::int_ >> lit(")")
					>> lit("{") >> shaderProgram >> lit("}");

				start = *intMatPair[phx::insert( phx::ref(shaderProgs), qi::_1 )];
			}

			qi::rule<std::string::const_iterator> comment;

			qi::rule<std::string::const_iterator, ShaderProgram::ShaderAutoBindType()> shaderAutoBindType;
			qi::rule<std::string::const_iterator, KgShaderProgramData::ShaderVar(), Skipper> shaderVariable;
			qi::rule<std::string::const_iterator, std::string(), Skipper> texture;
			qi::rule<std::string::const_iterator, VertexInputLayout::Type(), Skipper> vertexInputLayout;
			qi::rule<std::string::const_iterator, KgShaderProgramData::ShaderDef(), Skipper> vertexShaderDefinition;
			qi::rule<std::string::const_iterator, KgShaderProgramData::ShaderDef(), Skipper> pixelShaderDefinition;
			qi::rule<std::string::const_iterator, KgShaderProgramData(), Skipper> shaderProgram;
			qi::rule<std::string::const_iterator, std::pair<int, KgShaderProgramData>(), Skipper> intMatPair;

			qi::rule<std::string::const_iterator, Skipper> start;
		} passGrammar( shaderPrograms, vertLayoutTypes );

		Skipper skipper = iso8859::space | passGrammar.comment;

		std::string::const_iterator f = passString.cbegin();
		bool res = qi::phrase_parse( f, passString.cend(), passGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != passString.cend() )
		{
			std::string::const_iterator end = std::distance( f, passString.cend() ) > 100 ? f + 100 : passString.cend();
			std::cout << std::endl << "RenderPass parsing trail: " << std::string( f, end ) << std::endl;
			return false;
		}

		//TODO: add RenderPass to RenderCore

		VertexInputLayout vertLayout( vertLayoutTypes );
		// create ShaderPrograms
		std::map<int, KgShaderProgramData>::iterator matIt;
		for ( matIt = shaderPrograms.begin(); matIt != shaderPrograms.end(); ++matIt )
		{
			ShaderProgram *shaderProgram = ResourceManager::getInst()->createShaderProgram();

			// shaders should always be present under IOManager's search paths. No extra check is done if it isn't.
			std::string shaderPath = IOManager::getInst()->getAbsolutePath( matIt->second.vertexShader.filename );
			VertexShader *vertShader = shaderProgram->createVertexShader( shaderPath, vertLayout );
			setShaderVariables( shaderProgram, vertShader, matIt->second.vertexShader );
			// add vertex shader textures
			std::vector<std::string>::iterator it;
			for ( it = matIt->second.vertexShader.textures.begin(); it != matIt->second.vertexShader.textures.end(); ++it )
			{
				Texture *tex = TextureManager::getInst()->loadTexture( IOManager::getInst()->getAbsolutePath( *it ) );
				if ( tex )
					vertShader->addTexture( tex );
			}

			//TODO: add support for other shader types

			shaderPath = IOManager::getInst()->getAbsolutePath( matIt->second.pixelShader.filename );
			PixelShader *pixShader = shaderProgram->createPixelShader( shaderPath );
			setShaderVariables( shaderProgram, pixShader, matIt->second.pixelShader );
			// add pixel shader textures
			for ( it = matIt->second.pixelShader.textures.begin(); it != matIt->second.pixelShader.textures.end(); ++it )
			{
				Texture *tex = TextureManager::getInst()->loadTexture( IOManager::getInst()->getAbsolutePath( *it ) );
				if ( tex )
					pixShader->addTexture( tex );
			}

			//TODO: add support for multiple renderpasses
			renderCore->setShaderProgram( shaderProgram );
		}

		return true;
	}

	void KGSceneParser::setShaderVariables( ShaderProgram *ShaderProgram, ShaderBase *sh, const KgShaderProgramData::ShaderDef &shDef )
	{
		std::vector<KgShaderProgramData::ShaderVar>::const_iterator it;
		for ( it = shDef.variables.begin(); it != shDef.variables.end(); ++it )
		{
			if ( it->autoBindType != ShaderProgram::ShaderAutoBindType::NoAutoBind )
				ShaderProgram->addAutoShaderVar( sh, it->name, it->autoBindType );
			//TODO: add support for default constant values
			//else sh->updateConstantVariable( it->name, nullptr );
		}
	}
}
