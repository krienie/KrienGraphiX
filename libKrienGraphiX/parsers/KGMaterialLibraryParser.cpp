
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include "KGMaterialLibraryParser.h"

#include <comdef.h>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>

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


namespace phx     = boost::phoenix;
namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;


namespace kgx
{
	KGMaterialLibraryParser::KGMaterialLibraryParser()
	{
	}

	/*KGMaterialLibraryParser::~KGMaterialLibraryParser()
	{
	}*/

	bool KGMaterialLibraryParser::parse( const std::string &input, std::unordered_map<std::string, KgMatData> &outputData )
	{
		//TODO: output vertexLayoutTypes
		std::vector<VertexInputLayout::Type> vertLayoutTypes;

		struct MatLibGrammar : qi::grammar<std::string::const_iterator, Skipper>
		{
			MatLibGrammar( std::unordered_map<std::string, KgMatData> &shaderProgs, std::vector<VertexInputLayout::Type> &l )
				: base_type( start )
			{
				comment = "//" >> qi::skip(qi::blank)[*qi::print];

				float4Variable = qi::lit('(') >> qi::double_ >> qi::lit(',') >> qi::double_ >> qi::lit(',')
											  >> qi::double_ >> qi::lit(',') >> qi::double_ >> qi::lit(");");
				stringVariable = qi::lit('(') >> qi::lit('"') >> *~qi::char_('"') >> qi::lit('"') >> qi::lit(");");

				material = qi::lit( "Material" ) >> qi::lit( '(' ) >> *~qi::char_( ')' ) >> qi::lit( ')' )
					>> qi::lit( '{' )
					>> -(qi::lit("diffuse") >> float4Variable)
					>> -(qi::lit("diffusemap") >> stringVariable)
					>> -(qi::lit("specular") >> float4Variable)
					>> -(qi::lit("specularmap") >> stringVariable)
					>> -(qi::lit("normalmap") >> stringVariable)
					>> qi::lit('}');

				materialName = *~qi::char_(')');

				// look ahead to see what the name is of the material and use that as the key for the pair
				nameProgPair = qi::omit[ &(qi::lit("Material") >> qi::lit('(') >> materialName[qi::_a = qi::_1] >> qi::lit(')')) ]
								>> qi::attr(qi::_a) >> material;

				start = *nameProgPair[phx::insert( phx::ref(shaderProgs), qi::_1 )];
			}

			qi::rule<std::string::const_iterator> comment;

			qi::rule<std::string::const_iterator, DirectX::XMFLOAT4(), Skipper> float4Variable;
			qi::rule<std::string::const_iterator, std::string(), Skipper> stringVariable;
			qi::rule<std::string::const_iterator, KgMatData(), Skipper> material;
			qi::rule<std::string::const_iterator, std::string(), Skipper> materialName;
			qi::rule<std::string::const_iterator, qi::locals<std::string>, std::pair<std::string, KgMatData>(), Skipper> nameProgPair;

			qi::rule<std::string::const_iterator, Skipper> start;
		} matLibGrammar( outputData, vertLayoutTypes );

		Skipper skipper = iso8859::space | matLibGrammar.comment;

		std::string::const_iterator f = input.cbegin();
		qi::phrase_parse( f, input.cend(), matLibGrammar, skipper );

		// print everything that hasn't been processed by the parser
		if ( f != input.cend() )
		{
			std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "RenderPass parsing trail: " << std::string( f, end ) << std::endl;
			return false;
		}

		return true;
	}
}
