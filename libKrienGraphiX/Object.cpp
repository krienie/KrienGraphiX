
#include "Object.h"

namespace kgx
{
	Object::Object( const std::string &name )
		: m_name(name)
	{
	}

	Object::~Object()
	{
	}

	std::string Object::getName() const
	{
		return m_name;
	}
	void Object::setName( const std::string &name )
	{
		m_name = name;
	}
}
