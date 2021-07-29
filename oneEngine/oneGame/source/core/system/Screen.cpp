#include "Screen.h"

#include "core/debug.h"

#include <vector>

//namespace Screen
//{
//	_screen_info_t Info;
//
//	void _screen_info_t::Update ( void )
//	{
//		aspect = double( width )/ height;
//	}
//}

static std::vector<ArScreen> gScreenList;
static int gScreenFocused = 0;

void ArScreen::SetFocused ( bool focused )
{
	this->focused = focused;
	gScreenFocused = index;
}

ArScreen& core::GetScreen ( const int index )
{
	ARCORE_ASSERT(index >= 0 && index < gScreenList.size());

	return gScreenList[index];
}

ArScreen& core::GetFocusedScreen ( void )
{
	ARCORE_ASSERT(gScreenFocused >= 0 && gScreenFocused < gScreenList.size());

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
