
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
}
