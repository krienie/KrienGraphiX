
#include "IO/Filesystem.h"

#include <set>
#include <iostream>
#include <Windows.h>

#include <boost/filesystem.hpp>

namespace
{
	std::set<std::string> searchPaths;
}

namespace kgx { namespace filesystem
{
	std::string getCurrentProgramPath()
	{
		char result[MAX_PATH];
		std::string progPath = std::string( result, GetModuleFileName(nullptr, result, MAX_PATH ) );
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
		return searchPaths.erase(pathToRemove) == 1;
	}

	void clearSearchPaths()
	{
		searchPaths.clear();
	}

	std::string getFile( const std::string &filename )
	{
		std::string absFile = filename;
		if ( !boost::filesystem::path(filename).is_absolute() )
			absFile = getAbsolutePath(filename);
		if ( absFile.size() == 0 || !boost::filesystem::exists( absFile ) )
		{
			std::cout << "Error (Filesystem::openFile): Source file not found." << std::endl;
			return std::string();
		}

		return absFile;
	}

	bool openFile( const std::string &file, std::string &contents )
	{
		std::string absFile = getFile( file );
		if ( absFile.empty() )
			return false;

		std::ifstream in( absFile, std::ios::in | std::ios::binary );
		if ( in )
		{
			in.seekg( 0, std::ios::end );
			contents.resize( in.tellg() );
			in.seekg( 0, std::ios::beg );
			in.read( &contents[0], contents.size() );
			in.close();
			return true;
		}

		return false;
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
