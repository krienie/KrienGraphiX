
#include "MTLUtils.h"

#include <iostream>

#include <Foundation/NSDictionary.hpp>
#include <Foundation/NSError.hpp>
#include <Foundation/NSString.hpp>

#include "KrienGraphiX/Core/Logging.h"

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

	std::string errorString = std::format("--- NS::Error Details ---\n"
		"Domain: {0}\n"
		"Code: {1}\n"
		"Message: {2}\n", domain, code, description);

	if (NS::Dictionary* userInfo = error->userInfo())
	{
		if (NS::Object* reason = userInfo->object(NS::String::string("NSLocalizedFailureReason", NS::UTF8StringEncoding)))
		{
			const std::string reasonString = static_cast<NS::String*>(reason)->utf8String();
			errorString += std::format("Reason: {0}", reasonString);
		}
	}

	KGXLOG_ERROR(errorString);
}
}
