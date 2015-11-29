
#pragma warning( disable : 4996 )		// warning C4996: Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. These are correct.

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <sstream>

#include <ShaderProgram.h>

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
	(std::string, name)
	(kgx::KgMatData::ShaderDef, vertexShader)
	(kgx::KgMatData::ShaderDef, pixelShader)
);


namespace phx   = boost::phoenix;
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
		headerStr << "// KrienGraphiX Object v0.2 - (c)2015 Krien Linnenbank" << std::endl;
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
					<< lit( '\t' ) << no_delimit[(vertex % karma::space)[karma::_1 = phx::ref( inVerts )]] << no_delimit[eol]
					<< "}" << no_delimit[eol];

				indices = "Indices()" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< lit('\t') << no_delimit[(uint_ % karma::space)[karma::_1 = phx::ref( inIndices )]] << no_delimit[eol]
					<< "}" << no_delimit[eol];

				model = "Model(" << karma::string << ")" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\tIndices(" << no_delimit[int_] << "," << int_ << ")" << no_delimit[eol]
					<< "\tMaterial(" << karma::string << ")" << no_delimit[eol]
					<< "}" << no_delimit[eol];

				shaderAutoBindType = karma::stream;
				shaderVariable     = no_delimit[eol] << "\t\t" << shaderAutoBindType << karma::string << no_delimit[karma::string] << "(" << karma::string << ");" << no_delimit[eol << "\t"];

				vertexShader = "\tVertexShader(" << karma::string << no_delimit[")"] << no_delimit[eol]
								<< "\t{"
								<< *shaderVariable << no_delimit[eol]
								<< *("\t\tTexture(" << karma::string << ");" << no_delimit[eol]) << no_delimit[eol]
								<< "\t}" << no_delimit[eol];

				pixelShader  = "\tPixelShader(" << karma::string << no_delimit[")"] << no_delimit[eol]
								<< "\t{"
								<< *shaderVariable << no_delimit[eol]
								<< *("\t\tTexture(" << karma::string << ");" << no_delimit[eol])
								<< "\t}" << no_delimit[eol];

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
			karma::rule<BackInsertIt, ShaderProgram::ShaderAutoBindType()> shaderAutoBindType;
			karma::rule<BackInsertIt, KgMatData::ShaderVar(), karma::space_type> shaderVariable;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> vertexShader;
			karma::rule<BackInsertIt, KgMatData::ShaderDef(), karma::space_type> pixelShader;
			karma::rule<BackInsertIt, KgMatData(), karma::space_type> material;
		} kgoGram( headerStr.str(), inputData.inputLayout, inputData.vertices, inputData.indices, inputData.models, inputData.mats );


		BackInsertIt sink( outputString );
		bool result = karma::generate_delimited( sink, kgoGram, karma::space );
	}
}

#pragma warning( default : 4996 )	// restore default warning level
