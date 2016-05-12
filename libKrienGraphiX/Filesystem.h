
#pragma once

#include <string>

namespace kgx
{
	namespace filesystem
	{
			//TODO: make static
			void clearSearchPaths();

			std::string getCurrentProgramPath();
			std::string getAbsolutePath( const std::string &file );

			void addSearchPath( const std::string &absolutePath );
			bool removeSearchPath( const std::string &pathToRemove );

			bool saveFile( const std::string &fileDir, const std::string &fileName, const std::string &contents );
	};
}
