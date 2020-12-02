#ifndef DAWN_GUI_H_
#define DAWN_GUI_H_

#include "engine/behavior/CGameBehavior.h"

// DUSK-GUI is great, but is meant to copy OS-style controls. It is not console-friendly.
// DAWN-GUI is the console-friendly answer.
// Layouts are done mostly automatically, and control with a controller made usable.

namespace dawn
{
	class UserInterface : public CGameBehavior
	{
	public:
		bool IsMouseInside ( void )
		{
			return false;
		}
		
	};
}

#endif DAWN_GUI_H_