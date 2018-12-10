
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include "IO/KGMaterialLibraryGenerator.h"

#include <DirectXMath.h>

#include <comdef.h>
#include <sstream>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/karma_real.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "IO/KGMaterialLibraryParser.h"

BOOST_FUSION_ADAPT_STRUCT(
	DirectX::XMFLOAT4,
	(float, x)
	(float, y)
	(float, z)
	(float, w)
);

BOOST_FUSION_ADAPT_STRUCT(
	kgx::KgMatData,
	(std::string, name)
	(DirectX::XMFLOAT4, diffuse)
	(std::string, diffuseMap)
	(DirectX::XMFLOAT4, specular)
	(std::string, specularMap)
	(std::string, normalMap)
);

namespace phx   = boost::phoenix;
namespace karma = boost::spirit::karma;
typedef std::back_insert_iterator<std::string> BackInsertIt;


namespace kgx
{
	KGMaterialLibraryGenerator::KGMaterialLibraryGenerator()
	{
	}

	/*KGMaterialLibraryGenerator::~KGMaterialLibraryGenerator()
	{
	}*/

	void KGMaterialLibraryGenerator::generate( const std::vector<KgMatData> &inputData, std::string &output )
	{
		// construct header string
		std::stringstream headerStr;
		headerStr << "// KrienGraphiX Material Library v0.1 - (c)2017 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		headerStr.imbue( std::locale( headerStr.getloc(), facet ) );
		headerStr << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct KgmGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			KgmGram( const std::string &header, const std::vector<KgMatData> &inMatData )
				: base_type( output )
			{
				float4Variable = "(" << karma::no_delimit[karma::double_] << ","
									 << karma::no_delimit[karma::double_] << ","
									 << karma::no_delimit[karma::double_] << ","
									 << karma::double_ << ");";

				stringVariable = karma::lit('(') << karma::no_delimit[karma::lit('"')]
												 << karma::no_delimit[karma::string] << karma::lit('"')
												 << karma::lit(");") << karma::no_delimit[karma::eol];

				material = "Material(" << karma::string << ")" << karma::no_delimit[karma::eol]
					<< "{" << karma::no_delimit[karma::eol]
					<< karma::no_delimit[karma::lit("\tdiffuse")] << float4Variable << karma::no_delimit[karma::eol]
					<< karma::no_delimit[karma::lit("\tdiffusemap")] << stringVariable << karma::no_delimit[karma::eol]
					<< karma::no_delimit[karma::lit("\tspecular")] << float4Variable << karma::no_delimit[karma::eol]
					<< karma::no_delimit[karma::lit("\tspecularmap")] << stringVariable << karma::no_delimit[karma::eol]
					<< karma::no_delimit[karma::lit("\tnormalmap")] << stringVariable << karma::no_delimit[karma::eol]
					<< "}" << karma::no_delimit[karma::eol];

				materialVector = *material;

				output = header << karma::no_delimit[karma::eol] << materialVector[karma::_1 = phx::ref( inMatData )];
			}

			karma::rule<BackInsertIt, karma::space_type> output;
			karma::rule<BackInsertIt, DirectX::XMFLOAT4(), karma::space_type> float4Variable;
			karma::rule<BackInsertIt, std::string(), karma::space_type> stringVariable;
			karma::rule<BackInsertIt, KgMatData(), karma::space_type> material;
			karma::rule<BackInsertIt, std::vector<KgMatData>(), karma::space_type> materialVector;
		} kgoGram( headerStr.str(), inputData );


		BackInsertIt sink( output );
		generate_delimited( sink, kgoGram, karma::space );
	}


	void KGMaterialLibraryGenerator::append( const std::vector<KgMatData> &inputData, std::string &matLibString )
	{
		std::unordered_map<std::string, KgMatData> originalData;
		KGMaterialLibraryParser::parse( matLibString, originalData );

		std::unordered_map<std::string, KgMatData> combinedData;
		// combine input and original data and filter out any duplicate materials (filter by name, in favour of the original data)
		combinedData.insert( originalData.cbegin(), originalData.cend() );

		std::vector<KgMatData>::const_iterator it;
		for ( it = inputData.cbegin(); it != inputData.cend(); ++it )
			combinedData.insert( std::pair<std::string, KgMatData>(it->name, *it) );

		// convert map to vector
		std::vector<KgMatData> combinedVecData;
		std::unordered_map<std::string, KgMatData>::const_iterator convIt;
		for ( convIt = combinedData.cbegin(); convIt != combinedData.cend(); ++convIt )
			combinedVecData.push_back( convIt->second );

		matLibString.clear();
		generate( combinedVecData, matLibString );
	}

	std::string KGMaterialLibraryGenerator::float4ToString( const DirectX::XMFLOAT4 &float4 )
	{
		std::stringstream ss;
		ss << float4.x << ", " << float4.y << ", " << float4.z << ", " << float4.w;

		return ss.str();
	}
}
