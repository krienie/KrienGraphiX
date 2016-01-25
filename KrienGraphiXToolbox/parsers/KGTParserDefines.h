
#pragma once

#include <string>

namespace kgt
{
	struct KgProjectData
	{
		KgProjectData() : projectName(), sceneFile() {}

		std::string projectName;
		std::string sceneFile;
	};
}
