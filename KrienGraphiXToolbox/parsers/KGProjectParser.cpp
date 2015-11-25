
#ifndef BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_USE_PHOENIX_V3
#endif

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_iso8859_1.hpp>
#include <boost/filesystem.hpp>

#include "KGProjectParser.h"

BOOST_FUSION_ADAPT_STRUCT(
	kgt::KgProjectData,
	(std::string, projectName)
	(std::string, sceneFile)
);


namespace qi      = boost::spirit::qi;
namespace iso8859 = boost::spirit::iso8859_1;
typedef qi::rule<std::string::const_iterator> Skipper;

namespace kgt
{
	bool KGProjectParser::loadKGProject( const std::string &kgprojectFile, KgProjectData &projectData )
	{
		if ( kgprojectFile.size() == 0 )
		{
			std::cout << "Error (KGProjectParser::loadKGProject): Project file not specified." << std::endl;
			return false;
		}

		if ( !boost::filesystem::exists( kgprojectFile ) )
		{
			std::cout << "Error (KGProjectParser::loadKGProject): Project file does not exist." << std::endl;
			return false;
		}

		std::stringstream ssKgs;
		std::filebuf fb;
		if ( fb.open( kgprojectFile, std::ios::in ) )
		{
			ssKgs << &fb;
			fb.close();
		}


		struct KgprojectGrammar : qi::grammar<std::string::const_iterator, KgProjectData(), Skipper>
		{
			KgprojectGrammar()
				: KgprojectGrammar::base_type( start )
			{
				using namespace qi;

				comment = "//" >> skip( blank )[*print];

				start = "kgproject" >> lit("{")
					>> lit("name") >> lit("=") >> lexeme[lit('\"') >> *(alnum | space) >> lit('\"')]
					>> lit("sceneFile") >> lit("=") >> lexeme[lit('\"') >> *(alnum | char_('.')) >> lit('\"')]
					>> "}";
			}

			qi::rule<std::string::const_iterator, KgProjectData(), Skipper> start;
			qi::rule<std::string::const_iterator> comment;
		} kgprojGrammar;

		Skipper skipper = iso8859::space | kgprojGrammar.comment;

		std::string input = ssKgs.str();
		std::string::const_iterator f = input.cbegin();
		bool res = qi::phrase_parse( f, input.cend(), kgprojGrammar, skipper, projectData );

		// print everything that hasn't been processed by the parser
		if ( f != input.cend() )
		{
			std::string::const_iterator end = std::distance( f, input.cend() ) > 100 ? f + 100 : input.cend();
			std::cout << std::endl << "KGProject parsing trail:\n" << std::string( f, end ) << std::endl;
			return false;
		}

		return true;
	}
}
