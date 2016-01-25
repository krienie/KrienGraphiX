
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_SPIRIT_USE_PHOENIX_V3

#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <sstream>

#include "../parsers/KGTParserDefines.h"
#include "KGTProjectGenerator.h"


namespace karma = boost::spirit::karma;
namespace phx   = boost::phoenix;
typedef std::back_insert_iterator<std::string> BackInsertIt;

namespace kgt
{
	void KGTProjectGenerator::generate( const KgProjectData &projData, std::string &output )
	{
		// construct header string
		std::stringstream outSS;
		outSS << "// KrienGraphiX Project v0.1 - (c)2016 Krien Linnenbank" << std::endl;
		// get local date and time
		boost::posix_time::time_facet *facet = new boost::posix_time::time_facet( "%d-%b-%Y %H:%M:%S" );
		outSS.imbue( std::locale( outSS.getloc(), facet ) );
		outSS << "// File created: " << boost::posix_time::second_clock::local_time() << std::endl;

		struct ProjectGram : karma::grammar<BackInsertIt, karma::space_type>
		{
			ProjectGram( const std::string &projName, const std::string &sceneFile )
				: ProjectGram::base_type( output )
			{
				using namespace karma;

				output = "kgproject" << no_delimit[eol]
					<< "{" << no_delimit[eol]
					<< "\tname =" << no_delimit[karma::char_('\"') << karma::string(projName) << karma::char_('\"') << eol]
					<< "\tsceneFile =" << no_delimit[karma::char_('\"') << karma::string(sceneFile) << karma::char_('\"') << eol]
					<< "}" << no_delimit[eol];
			}

			karma::rule<BackInsertIt, karma::space_type> output;

		} projGram( projData.projectName, projData.sceneFile );


		std::string projString;
		BackInsertIt sink( projString );
		bool result = karma::generate_delimited( sink, projGram, karma::space );

		outSS << projString;
		output = outSS.str();
	}
}
