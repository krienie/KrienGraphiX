
#pragma warning( disable : 4996 )		// warning C4996: Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. These are correct.

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <sstream>

#include <Material.h>

#include "KGObjectGenerator.h"


BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderVar,
	(kgx::Material::ShaderAutoBindType, autoBindType)
	(std::string, type)
	(std::string, name)
	(std::string, defaultValue)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData::ShaderDef,
	(std::string, name)
	(std::vector<kgx::KgMatData::ShaderVar>, variables)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(std::string, name)
	(kgx::KgMatData::ShaderDef, vertexShader)
	(kgx::KgMatData::ShaderDef, pixelShader)
);


namespace phx = boost::phoenix;
namespace karma = boost::spirit::karma;
typedef std::back_insert_iterator<std::string> BackInsertIt;


namespace kgx
{
	KGObjectGenerator::KGObjectGenerator()
	{
	}

	/*KGObjectGenerator::~KGObjectGenerator()
	{
	}*/

	void KGObjectGenerator::generate( _In_ KgoData &inputData, _Inout_ std::string &outputString )
	{
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX Object v0.1 - (c)2015 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		headerStr.imbue( std::locale( headerStr.getloc(), facet ) );
		headerStr << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct KgoGram : karma::grammar < BackInsertIt, karma::space_type >
		{
			KgoGram( const std::string &header, const std::vector<VertexInputLayout::Type> &inLayout,
					 const std::vector<float> &inVerts, const std::vector<UINT> &inIndices,
					 const std::vector<KgModelData> &inModels, const std::vector<KgMatData> &inMatData )
					 : KgoGram::base_type( output )
			{
				using namespace karma;
				inputLayoutType = karma::stream;
				inputLayout = (inputLayoutType % ", ")[karma::_1 = phx::ref( inLayout )];

				vertex = double_;
				vertices = "Vertices(" << inputLayout << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< lit( '\t' ) << no_delimit[(vertex % karma::space)[karma::_1 = phx::ref( inVerts )]] << ";" << no_delimit[eol]
					<< "}" << no_delimit[eol];

				indices = "Indices()" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< lit('\t') << no_delimit[(uint_ % karma::space)[karma::_1 = phx::ref( inIndices )]] << ";" << no_delimit[eol]
					<< "}" << no_delimit[eol];

				model = "Model(" << karma::string << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\tIndices(" << no_delimit[int_] << "," << int_ << ");" << no_delimit[eol]
					<< "\tMaterial(" << karma::string << ");" << no_delimit[eol]
					<< "}" << no_delimit[eol];

				shaderAutoBindType = karma::stream;
				shaderVariable     = no_delimit[eol] << "\t\t" << shaderAutoBindType << karma::string << no_delimit[karma::string] << "(" << karma::string << ");" << no_delimit[eol << "\t"];

				vertexShader = "\tVertexShader(" << karma::string << no_delimit[")"] << no_delimit[eol] << "\t{" << *shaderVariable << "}" << no_delimit[eol];
				pixelShader  = "\tPixelShader(" << karma::string << no_delimit[")"] << no_delimit[eol] << "\t{" << *shaderVariable << "}" << no_delimit[eol];

				material = "Material(" << karma::string << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< vertexShader << no_delimit[eol] << pixelShader
					<< "}" << no_delimit[eol];


				output = header << no_delimit[eol] << vertices << no_delimit[eol] << indices << no_delimit[eol]
					<< (*model)[karma::_1 = phx::ref( inModels )] << no_delimit[eol] << (*material)[karma::_1 = phx::ref( inMatData )];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt> inputLayout;
			karma::rule<BackInsertIt, VertexInputLayout::Type()> inputLayoutType;
			karma::rule<BackInsertIt, float()> vertex;
			karma::rule<BackInsertIt, karma::space_type> vertices;
			karma::rule<BackInsertIt, karma::space_type> indices;
			karma::rule<BackInsertIt, KgModelData(), karma::space_type> model;
			karma::rule<BackInsertIt, Material::ShaderAutoBindType()> shaderAutoBindType;
			karma::rule<BackInsertIt, KgMatData::ShaderVar(), karma::space_type> shaderVariable;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> vertexShader;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> pixelShader;
			karma::rule<BackInsertIt, KgMatData(), karma::space_type> material;
		} kgoGram( headerStr.str(), inputData.inputLayout, inputData.vertices, inputData.indices, inputData.models, inputData.mats );


		BackInsertIt sink( outputString );
		bool result = karma::generate_delimited( sink, kgoGram, karma::space );
		//std::cout << "kgoFile: " << std::endl << outputString << std::endl;
		//std::cout << std::endl;
	}

	/*bool generateKGM( _Inout_ BackInsertIt &sink, _In_ KgmData &input, _In_ const std::string &texturePath )
	{
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX KGM Converter v" << Version << " - (c)2014 Krien Linnenbank" << std::endl;
		// create static comment strings
		std::string vertStr = "// Vertex signature: Position(x,y,z) TextureCoordinate(x,y,z) Normal(x,y,z)";
		std::string idxStr = "// Model indices";
		std::string matStr = "// Material signature: Name SpecularPower OpticalDensity Transparency TransmissionFilter(x,y,z) AmbientColor(x,y,z) DiffuseColor(x,y,z) SpecularColor(x,y,z) EmissiveColor(x,y,z) AmbientMap DiffuseMap SpecularMap TransparencyMap BumpMap";
		std::string modStr = "// Model signature: Name StartIndex IndexCount MaterialName";

		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		headerStr.imbue( std::locale( headerStr.getloc(), facet ) );
		headerStr << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct KgmGram
			: karma::grammar<BackInsertIt, KgmData(), karma::space_type>
		{
			KgmGram( const std::string &header, const std::string &vStr, const std::string &iStr,
						const std::string &mtStr, const std::string &mdStr, const std::string &tPath )
						: KgmGram::base_type( output )
			{
				using namespace karma;
				headerComment = karma::string( header ) << no_delimit[eol];
				vertComment = karma::string( vStr ) << no_delimit[eol];
				idxComment = karma::string( iStr ) << no_delimit[eol];
				matComment = karma::string( mtStr ) << no_delimit[eol];
				modComment = karma::string( mdStr ) << no_delimit[eol];

				vertices = headerComment << vertComment
					<< *('v' << *double_) << no_delimit[eol << eol];
				indices = idxComment << 'i' << *uint_ << no_delimit[eol << eol];
				vector = repeat( phx::bind( &std::vector<float>::size, _val ) )[double_]
					<< repeat( 3 - phx::bind( &std::vector<float>::size, _val ) )[lit( "0.0" )];

				//eps(phx::bind(&std::string::size, _val) > 0)
				texMap = (eps( phx::bind( &std::string::size, _val ) > 0 ) << no_delimit['\"' << lit( tPath ) << karma::string] << '\"')
					| karma::lit( "\"\"" );
				materials = matComment << *("mat" << karma::string << double_ << double_ << double_
												<< vector << vector << vector << vector << vector
												<< texMap << texMap << texMap << texMap << texMap << no_delimit[eol])
												<< no_delimit[eol];
				models = modComment << *("mod" << karma::string << int_ << int_ << karma::string << no_delimit[eol]) << no_delimit[eol];

				output = vertices << indices << materials << models;
			}

			karma::rule<BackInsertIt, KgmData(), karma::space_type> output;
			karma::rule<BackInsertIt, std::vector<float>(), karma::space_type> vertices;
			karma::rule<BackInsertIt, std::vector<UINT>(), karma::space_type> indices;
			karma::rule<BackInsertIt, std::vector<KgModelData>(), karma::space_type> models;
			karma::rule<BackInsertIt, std::vector<KgMatData>(), karma::space_type> materials;
			karma::rule<BackInsertIt, std::vector<float>(), karma::space_type> vector;
			karma::rule<BackInsertIt, std::string(), karma::space_type> texMap;
			karma::rule<BackInsertIt, karma::space_type> headerComment, vertComment, idxComment, matComment, modComment;
		} kgmGram( headerStr.str(), vertStr, idxStr, matStr, modStr, texturePath );

		return karma::generate_delimited( sink, kgmGram, karma::space, input );
	}*/
}

