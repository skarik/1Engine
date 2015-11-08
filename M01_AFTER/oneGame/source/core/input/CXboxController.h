
#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>

namespace InputControl
{

	// Now, the XInput Library
	// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING
	// A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
	//#pragma comment(lib, "XInput.lib")

	// XBOX Controller Class Definition
	class CXBoxController
	{
	private:
		XINPUT_STATE _controllerState;
		int _controllerNum;
	public:
		// Constructor
		CXBoxController( int playerNumber );

		XINPUT_STATE GetState();

		bool IsConnected();
		void Vibrate(int leftVal = 0, int rightVal = 0);
	};

}
#endif//_XBOX_CONTROLLER_H_