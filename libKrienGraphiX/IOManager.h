
#include <string>
#include <set>

namespace kgx
{
	class IOManager
	{
		public:
			static IOManager* getInst();
			static void destroy();

			std::string getAbsolutePath( const std::string &file ) const;

			void addSearchPath( const std::string &absolutePath );
			void clearSearchPaths();

		private:
			IOManager();
			//~IOManager();

			// no copying allowed
			IOManager( const IOManager& );
			IOManager& operator=( const IOManager& );

			static IOManager* m_inst;

			std::set<std::string> m_searchPaths;
	};
}
