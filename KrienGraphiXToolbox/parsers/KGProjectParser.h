
#pragma once

#include <winerror.h>
#include <string>

namespace kgt
{
	struct KgProjectData
	{
		KgProjectData() : projectName(), sceneFile() {}

		std::string projectName;
		std::string sceneFile;
	};

	class KGProjectParser
	{
		public:
			static bool loadKGProject( const std::string &kgprojectFile, KgProjectData &projectData );

		private:
			KGProjectParser();
			~KGProjectParser();

			// no copying allowed
			KGProjectParser( const KGProjectParser& );
			KGProjectParser& operator=(const KGProjectParser&);
	};
}
