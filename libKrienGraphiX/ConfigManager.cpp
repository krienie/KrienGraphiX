
#include <iostream>

#include "Filesystem.h"
#include "ConfigManager.h"

namespace bpt = boost::property_tree;

namespace kgx
{
	ConfigManager* ConfigManager::m_inst = nullptr;

	ConfigManager* ConfigManager::getInst()
	{
		if ( !m_inst )
			m_inst = new ConfigManager();

		return m_inst;
	}

	void ConfigManager::destroy()
	{
		if ( ConfigManager::m_inst )
			delete ConfigManager::m_inst;
	}


	ConfigManager::ConfigManager()
		: m_iniTree(), m_iniFilePath()
	{
		m_iniFilePath = filesystem::getAbsolutePath( INI_FILENAME );
		if ( m_iniFilePath.size() > 0 )
			bpt::ini_parser::read_ini( m_iniFilePath, m_iniTree );
	}

	/*ConfigManager::~ConfigManager()
	{
	}*/
}
