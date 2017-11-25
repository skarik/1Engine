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

#include "core/types.h"
#include "core/math/Math3d.h"

#include "core/os.h"
#include <XInput.h>

namespace InputControl
{
	enum arAnalogIndex
	{
		kAnalogIndexLeftStick,
		kAnalogIndexRightStick,
		kAnalogIndexTriggers,
		kAnalogIndexBumpers,
	};

	//	CXboxController : XBOX Controller Class
	// Provides interface to Win32's XInput as well as utility functions.
	class CXBoxController
	{
	private:
		XINPUT_STATE	controllerState;
		int				controllerIndex;
		bool			controllerValid;
#		ifdef _ENGINE_DEBUG
		bool			controllerInputUpdated;
#		endif

	public:
		//	Constructor( player_index ) : Creates XBox controller interface.
		// player_index : Index of 0 to 3 of the player.
		CXBoxController ( int playerIndex );

		CORE_API void					UpdateState ( void );
		CORE_API const XINPUT_STATE&	GetState ( void );
		CORE_API bool					IsConnected ( void );

		CORE_API void					Vibrate ( int leftVal = 0, int rightVal = 0 );

		//	GetAnalog( axis_group ) : Returns the analog input of given group.
		// Inputs are normalized to the -1 to +1 range.
		// Triggers are normally only in the 0 to +1 range.
		CORE_API Vector3d				GetAnalog ( const arAnalogIndex analogIndex );
	};

}

#endif
#endif//XBOX_CONTROLLER_H_