
#include <sstream>
#include <iostream>

#include <boost/filesystem.hpp>

#include "IOManager.h"

namespace kgx
{
	IOManager* IOManager::m_inst = nullptr;

	IOManager* IOManager::getInst()
	{
		if ( !m_inst )
			m_inst = new IOManager();

		return m_inst;
	}


	void IOManager::destroy()
	{
		if ( IOManager::m_inst )
			delete IOManager::m_inst;
	}

	IOManager::IOManager()
		: m_searchPaths()
	{

	}

	/*IOManager::~IOManager()
	{
	}*/


	std::string IOManager::getAbsolutePath( const std::string &file ) const
	{
		std::set<std::string>::const_iterator it;
		for ( it = m_searchPaths.cbegin(); it != m_searchPaths.cend(); ++it )
		{
			//TODO: consider making this non-recursive and make addSearchPath recursive..
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


	void IOManager::addSearchPath( const std::string &absolutePath )
	{
		boost::filesystem::path p( absolutePath );
		if ( boost::filesystem::is_directory( p ) )
			m_searchPaths.insert( absolutePath );
	}

	void IOManager::clearSearchPaths()
	{
		m_searchPaths.clear();
	}


	bool IOManager::saveFile( const std::string &fileDir, const std::string &fileName, const std::string &contents ) const
	{
		if ( !boost::filesystem::is_directory(fileDir) )
		{
			std::cout << "Error (IOManager::saveFile): " << fileDir << " is not a valid directory. Aborting" << std::endl;
			return false;
		}

		boost::filesystem::path dir( fileDir );
		if ( dir.is_relative() || !boost::filesystem::exists(dir) && !boost::filesystem::create_directory(dir) )
		{
			std::cout << "Error (IOManager::saveFile): Error creating directory: " << fileDir << " Aborting" << std::endl;
			return false;
		}

		boost::filesystem::path file( fileName );
		boost::filesystem::path fullPath = dir / file;

		if ( boost::filesystem::exists(fullPath) )
			std::cout << "Warning (IOManager::saveFile): File already exists. Overwriting..." << std::endl;

		std::ofstream fileOut( fullPath.string() );
		fileOut << contents;
		fileOut.close();
		return true;
	}
}
