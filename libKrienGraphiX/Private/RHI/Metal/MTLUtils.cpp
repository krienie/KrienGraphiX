
#include "MTLUtils.h"

#include <iostream>

#include "Foundation/NSDictionary.hpp"
#include "Foundation/NSError.hpp"
#include "Foundation/NSString.hpp"

namespace kgx::RHI::MTLUtils
{
void printIfNSError(NS::Error* error)
{
	if (!error)
	{
		return;
	}

	const char* description = error->localizedDescription()->utf8String();
	const char* domain = error->domain()->utf8String();
	const NS::Integer code = error->code();

	std::cerr << "--- NS::Error Details ---" << std::endl;
	std::cerr << "Domain:  " << domain << std::endl;
	std::cerr << "Code:    " << code << std::endl;
	std::cerr << "Message: " << description << std::endl;

	if (NS::Dictionary* userInfo = error->userInfo())
	{
		if (NS::Object* reason = userInfo->object(NS::String::string("NSLocalizedFailureReason", NS::UTF8StringEncoding)))
		{
			const char* reasonStr = static_cast<NS::String*>(reason)->utf8String();
			std::cerr << "Reason:  " << reasonStr << std::endl;
		}
	}
}
}
