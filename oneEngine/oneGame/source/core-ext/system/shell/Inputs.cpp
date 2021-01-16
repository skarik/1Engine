#include "Inputs.h"
#include "core/os.h"

#if PLATFORM_WINDOWS

const float core::shell::GetDoubleClickInterval ( void )
{
	return GetDoubleClickTime() / 1000.0F;
}

#endif