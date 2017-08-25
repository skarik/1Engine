#include "Screen.h"

namespace Screen
{
	_screen_info_t Info;

	void _screen_info_t::Update ( void )
	{
		aspect = double( width )/ height;
	}
}