#include "Screen.h"

#include "core/debug.h"

#include <vector>

static std::vector<ArScreen>	gScreenList;
static int						gScreenFocused = 0;

bool core::GetScreenExists ( void )
{
	return !gScreenList.empty();
}

void ArScreen::SetFocused ( bool focused )
{
	this->focused = focused;
	gScreenFocused = index;
}

ArScreen& core::GetScreen ( const int index )
{
	ARCORE_ASSERT(index >= 0 && index < (int)gScreenList.size());

	return gScreenList[index];
}

ArScreen& core::GetFocusedScreen ( void )
{
	ARCORE_ASSERT(gScreenFocused >= 0 && gScreenFocused < (int)gScreenList.size());

	return gScreenList[gScreenFocused];
}

void core::SetScreenCount ( const int count )
{
	ARCORE_ASSERT(count >= 0);

	if (gScreenList.size() != count)
	{
		gScreenList.resize(count);
	}
}
