
#pragma once

#include <boost/property_tree/ptree.hpp>

namespace kgx
{
	class ConfigManager
	{
	public:
		static ConfigManager* getInst();
		static void destroy();

		template<typename T> T getProperty( const std::string &propName ) const;
		template<typename T> T getProperty( const std::string &sectionName, const std::string &propName ) const;
		template<typename T> void setProperty( const std::string &propName, T value );
		template<typename T> void setProperty( const std::string &sectionName, const std::string &propName, T value );

	private:
		ConfigManager();
		//~ConfigManager();

		// no copying allowed
		ConfigManager( const ConfigManager& );
		ConfigManager& operator=(const ConfigManager&);

		static ConfigManager* m_inst;

		boost::property_tree::ptree m_iniTree;
		std::string m_iniFilePath;
	};
}
