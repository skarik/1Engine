#include "core-ext.h"
#include <functional>
#include <vector>

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