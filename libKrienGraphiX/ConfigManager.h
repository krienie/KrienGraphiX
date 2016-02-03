
#pragma once

#include <boost/optional/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace kgx
{
	class ConfigManager
	{
		public:
			static ConfigManager* getInst();
			static void destroy();

			template<typename T>
			T getProperty( const std::string &propName ) const
			{
				return getProperty<T>( "general", propName );
			}
			template<typename T>
			T  getProperty( const std::string &sectionName, const std::string &propName ) const
			{
				std::stringstream ss;
				ss << sectionName << '.' << propName;

				// check if property exists
				boost::optional< const boost::property_tree::ptree& > child = m_iniTree.get_child_optional( ss.str() );
				if ( child )
					return m_iniTree.get<T>( ss.str() );

				return T();
			}

			template<typename T>
			void setProperty( const std::string &propName, T value )
			{
				setProperty<T>( "general", propName, value );
			}
			template<typename T>
			void setProperty( const std::string &sectionName, const std::string &propName, T value )
			{
				std::stringstream ss;
				ss << sectionName << '.' << propName;
				m_iniTree.put<T>( ss.str(), value );

				if ( m_iniFilePath.size() == 0 )
				{
					m_iniFilePath = IOManager::getInst()->getCurrentProgramPath();

					// write ini file
					std::stringstream fileSS;
					fileSS << m_iniFilePath << "\\" << INI_FILENAME;
					boost::property_tree::ini_parser::write_ini( fileSS.str(), m_iniTree );
				}
			}

		private:
			ConfigManager();
			//~ConfigManager();

			// no copying allowed
			ConfigManager( const ConfigManager& );
			ConfigManager& operator=(const ConfigManager&);

			static ConfigManager* m_inst;

			const std::string INI_FILENAME = "kgsettings.ini";

			boost::property_tree::ptree m_iniTree;
			std::string m_iniFilePath;
	};
}
