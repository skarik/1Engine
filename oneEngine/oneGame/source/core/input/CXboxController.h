//===============================================================================================//
//
//		CXboxController
//
// Simple layer between XInput and CInput.
//
//===============================================================================================//
#ifndef XBOX_CONTROLLER_H_
#define XBOX_CONTROLLER_H_
#ifdef WIN32

// We need the Windows Header and the XInput Header
#include "core/os.h"
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

#endif
#endif//XBOX_CONTROLLER_H_