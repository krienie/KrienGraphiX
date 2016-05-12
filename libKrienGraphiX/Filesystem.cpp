
#include <set>
#include <sstream>
#include <iostream>
#include <Windows.h>

#include <boost/filesystem.hpp>

#include "Filesystem.h"

namespace
{
	std::set<std::string> searchPaths;
}

namespace kgx { namespace filesystem
{

	std::string getCurrentProgramPath()
	{
		char result[MAX_PATH];
		std::string progPath = std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
		return progPath.substr( 0, progPath.find_last_of( "/\\" ) );
	}

	std::string getAbsolutePath( const std::string &file )
	{
		//TODO: handle files with the same name, but in different search paths
		addSearchPath( getCurrentProgramPath() );

		std::set<std::string>::const_iterator it;
		for ( it = searchPaths.cbegin(); it != searchPaths.cend(); ++it )
		{
			boost::filesystem::recursive_directory_iterator dirIt( *it ), eod;
			while ( dirIt != eod )
			{
				boost::filesystem::path curFile = dirIt->path();
				if ( boost::filesystem::is_regular_file( curFile )
						&& curFile.filename().string() == file )
				{
					return curFile.string();
				}

				++dirIt;
			}
		}

		return std::string();
	}


	void addSearchPath( const std::string &absolutePath )
	{
		boost::filesystem::path p( absolutePath );
		if ( boost::filesystem::is_directory( p ) )
			searchPaths.insert( absolutePath );
	}

	bool removeSearchPath( const std::string &pathToRemove )
	{
		return 1 == searchPaths.erase( pathToRemove );
	}

	void clearSearchPaths()
	{
		searchPaths.clear();
	}


	bool saveFile( const std::string &fileDir, const std::string &fileName, const std::string &contents )
	{
		if ( !boost::filesystem::is_directory(fileDir) )
		{
			std::cout << "Error (filesytem::saveFile): " << fileDir << " is not a valid directory. Aborting" << std::endl;
			return false;
		}

		boost::filesystem::path dir( fileDir );
		if ( dir.is_relative() || !boost::filesystem::exists(dir) && !boost::filesystem::create_directory(dir) )
		{
			std::cout << "Error (filesytem::saveFile): Error creating directory: " << fileDir << " Aborting" << std::endl;
			return false;
		}

		boost::filesystem::path file( fileName );
		boost::filesystem::path fullPath = dir / file;

		if ( boost::filesystem::exists(fullPath) )
			std::cout << "Warning (filesytem::saveFile): File already exists. Overwriting..." << std::endl;

		std::ofstream fileOut( fullPath.string() );
		fileOut << contents;
		fileOut.close();
		return true;
	}
}}
