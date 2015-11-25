
#pragma once

#include <string>

namespace kgx
{
	class Object
	{
		public:
			explicit Object( const std::string &name );
			virtual ~Object();

			std::string getName() const;
			void setName( const std::string &name );

		private:
			std::string m_name;
	};
}
