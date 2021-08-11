#ifndef CORE_EXT_LOCALE_LOCALE_H_
#define CORE_EXT_LOCALE_LOCALE_H_

#include "core/types.h"
#include <string>

// Localization/Translation system is a big TODO
// For now we provide passthrough stubs so translation can be added in easily later.

namespace core
{
	CORE_API const char*
						Localize ( const char* input )
		{ return input; }

	CORE_API const char*
						Localize ( const std::string& input )
		{ return input.c_str(); }
};

#endif//CORE_EXT_LOCALE_LOCALE_H_