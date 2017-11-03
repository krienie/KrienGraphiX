
#pragma once

#include <string>

namespace kgx
{
	namespace filesystem
	{
		void clearSearchPaths();

		std::string getCurrentProgramPath();
		std::string getAbsolutePath( const std::string &file );

		void addSearchPath( const std::string &absolutePath );
		bool removeSearchPath( const std::string &pathToRemove );

		std::string getFile( const std::string &filename );
		bool openFile( const std::string &file, std::string &contents );
		bool saveFile( const std::string &fileDir, const std::string &fileName, const std::string &contents );
	};
}
