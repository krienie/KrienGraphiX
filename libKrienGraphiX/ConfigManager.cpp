
#include <iostream>

#include <boost/property_tree/ini_parser.hpp>

#include "IOManager.h"
#include "ConfigManager.h"

namespace bpt = boost::property_tree;

namespace
{
	const std::string INI_FILENAME = "kgsettings.ini";
}

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
		m_iniFilePath = IOManager::getInst()->getAbsolutePath( INI_FILENAME );
		if ( m_iniFilePath.size() > 0 )
			bpt::ini_parser::read_ini( m_iniFilePath, m_iniTree );
	}

	/*ConfigManager::~ConfigManager()
	{
	}*/


	template<typename T>
	T ConfigManager::getProperty( const std::string &propName ) const
	{
		return getProperty<T>( "general", propName );
	}
	template<typename T>
	T ConfigManager::getProperty( const std::string &sectionName, const std::string &propName ) const
	{
		std::stringstream ss;
		ss << sectionName << '.' << propName;
		return m_iniTree.get<T>( ss.str() );
	}

	template<typename T>
	void ConfigManager::setProperty( const std::string &propName, T value )
	{
		setProperty<T>( "general", propName, value );
	}
	template<typename T>
	void ConfigManager::setProperty( const std::string &sectionName, const std::string &propName, T value )
	{
		std::stringstream ss;
		ss << sectionName << '.' << propName;
		m_iniTree.put<T>( ss.str(), value );

		if ( m_iniFilePath.size() == 0 )
			m_iniFilePath = IOManager::getInst()->getCurrentProgramPath();

		// write ini file
		std::stringstream fileSS;
		fileSS << m_iniFilePath << "\\" << INI_FILENAME;
		bpt::ini_parser::write_ini( fileSS.str(), m_iniTree );
	}
}
