
#include "Core/ConfigManager.h"

#include "IO/Filesystem.h"

namespace bpt = boost::property_tree;

namespace kgx
{
	ConfigManager* ConfigManager::m_inst = nullptr;

	ConfigManager* ConfigManager::get()
	{
		if ( !m_inst )
			m_inst = new ConfigManager();

		return m_inst;
	}

	void ConfigManager::destroy()
	{
		if ( m_inst )
			delete m_inst;
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
