//===============================================================================================//
//
//		core/utils/stringcase.h - Helper for switch-case logic with strings
//
//===============================================================================================//
#ifndef CORE_UTIL_STRINGCASE_H_
#define CORE_UTIL_STRINGCASE_H_

#include "core/common.h"
#include "core/containers/arstring.h"
#include "core/utils/stringhash.h"

namespace core
{
	namespace internals
	{
		constexpr size_t			arStringSwitchHash ( const char* str )
		{
			return arHashString(str);
		}

		template <unsigned short Ln>
		constexpr size_t			arStringSwitchHash ( arstring<Ln> str )
		{
			return arHashString(str.c_str());
		}
	}
}

#define string_switch(VALUE)	switch( core::internals::arStringSwitchHash( VALUE ) )
#define string_case(VALUE)		case core::internals::arStringSwitchHash( VALUE )

#define string_switch_begin_suppress_warnings()	PRAGMA(warning(push)) PRAGMA(warning(disable: 4307))
#define string_switch_end_suppress_warnings()	PRAGMA(warning(pop))

#endif//CORE_UTIL_STRINGCASE_H_