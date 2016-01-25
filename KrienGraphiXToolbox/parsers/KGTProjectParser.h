
#pragma once

#include <string>

#include "KGTParserDefines.h"

namespace kgt
{
	class KGTProjectParser
	{
		public:
			static bool loadKGProject( const std::string &kgprojectFile, KgProjectData &projectData );

		private:
			KGTProjectParser();
			~KGTProjectParser();

			// no copying allowed
			KGTProjectParser( const KGTProjectParser& );
			KGTProjectParser& operator=(const KGTProjectParser&);
	};
}
