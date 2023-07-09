#include "core-ext.h"

#include <functional>
#include <vector>

#include "core/os.h"
#include "core/debug.h"

static std::vector<std::function<void()>> m_applicationStartFunctions;
static std::vector<std::function<void()>> m_applicationEndFunctions;

void core::_RegisterOnApplicationStartup( std::function<void()> Fn )
{
	m_applicationStartFunctions.push_back(Fn);
}
void core::_RegisterOnApplicationEnd( std::function<void()> Fn )
{
	m_applicationEndFunctions.push_back(Fn);
}

void core::OnApplicationStartup ( void )
{
#if PLATFORM_WINDOWS
	_CrtSetDbgFlag(
		  _CRTDBG_ALLOC_MEM_DF // Enable debug allocation
		| _CRTDBG_CHECK_CRT_DF // Enable internal C Runtime checks
		| _CRTDBG_LEAK_CHECK_DF // Enable leak check at program exit
		);
#endif

	for (auto& fn : m_applicationStartFunctions)
	{
		fn();
	}
}

void core::OnApplicationEnd ( void )
{
	for (auto& fn : m_applicationEndFunctions)
	{
		fn();
	}
}

void core::OnApplicationGlobalTick ( void )
{
#if PLATFORM_WINDOWS
	
	ARCORE_ASSERT(_CrtCheckMemory());

#endif
}
