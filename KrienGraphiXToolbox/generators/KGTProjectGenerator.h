
#pragma once

#include <string>

#include "../parsers/KGTParserDefines.h"

namespace kgt
{
	class KGTProjectGenerator
	{
		public:
			static void generate( const KgProjectData &projData, std::string &output );

		private:
			KGTProjectGenerator();
			//~KGTProjectGenerator();

			// no copying allowed
			KGTProjectGenerator( const KGTProjectGenerator& );
			KGTProjectGenerator& operator=(const KGTProjectGenerator&);
	};
}
