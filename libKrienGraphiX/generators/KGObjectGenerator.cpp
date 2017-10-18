
#pragma warning( disable : 4996 )		// warning C4996: Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. These are correct.

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <comdef.h>
#include <sstream>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "../RenderableObject.h"
#include "KGObjectGenerator.h"

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgModelData,
	(std::string, modelName)
	(int, startIndex)
	(int, indexCount)
	(std::string, matName)
);

namespace phx   = boost::phoenix;
namespace karma = boost::spirit::karma;
typedef std::back_insert_iterator<std::string> BackInsertIt;

template <typename Num>
struct scientific_policy : karma::real_policies<Num>
{
	// we want the numbers always to be in scientific format
	static int floatfield(Num n) { return 1; }
};
typedef karma::real_generator<double, scientific_policy<double> > science_type;


namespace kgx
{
	KGObjectGenerator::KGObjectGenerator()
	{
	}

	/*KGObjectGenerator::~KGObjectGenerator()
	{
	}*/

	void KGObjectGenerator::generate( KgoData &inputData, std::string &outputString )
	{
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX Object v0.4 - (c)2017 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		headerStr.imbue( std::locale( headerStr.getloc(), facet ) );
		headerStr << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct KgoGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			KgoGram( const std::string &header, const std::vector<VertexInputLayout::Type> &inLayout,
					 const std::vector<float> &inVerts, const std::vector<UINT> &inIndices,
					 const std::vector<KgModelData> &inModels )
					 : base_type( output )
			{
				inputLayoutType = karma::stream;
				inputLayout = (inputLayoutType % ", ")[karma::_1 = phx::ref( inLayout )];


				science_type scienceDouble;

				vertex = scienceDouble;
				vertices = "Vertices(" << inputLayout << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::lit('\t') << karma::no_delimit[(vertex % karma::space)[karma::_1 = phx::ref(inVerts)]] << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				indices = "Indices()" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::lit('\t') << karma::no_delimit[(karma::uint_ % karma::space)[karma::_1 = phx::ref(inIndices)]] << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				model = "Model(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< "\tIndices(" << karma::no_delimit[karma::int_] << "," << karma::int_ << ")" << karma::no_delimit[karma::eol]
					<< "\tMaterial(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				output = header << karma::no_delimit[karma::eol] << vertices << karma::no_delimit[karma::eol] << indices << karma::no_delimit[karma::eol]
					<< (*model)[karma::_1 = phx::ref(inModels)] << karma::no_delimit[karma::eol];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt> inputLayout;
			karma::rule<BackInsertIt, VertexInputLayout::Type()> inputLayoutType;
			karma::rule<BackInsertIt, float()> vertex;
			karma::rule<BackInsertIt, karma::space_type> vertices;
			karma::rule<BackInsertIt, karma::space_type> indices;
			karma::rule<BackInsertIt, KgModelData(), karma::space_type> model;
		} kgoGram( headerStr.str(), inputData.inputLayout, inputData.vertices, inputData.indices, inputData.models );

		BackInsertIt sink( outputString );
		karma::generate_delimited( sink, kgoGram, karma::space );
	}
}

#pragma warning( default : 4996 )	// restore default warning level
