#include "CXBOXController.h"

using namespace InputControl;

CXBoxController::CXBoxController(int playerIndex)
	: controllerIndex(playerIndex), controllerValid(false)
{
	UpdateState();
#	ifdef _ENGINE_DEBUG
	controllerInputUpdated = false;
#	endif
}

void CXBoxController::UpdateState ( void )
{
	// Zeroise the state
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD result = XInputGetState(controllerIndex, &controllerState);
	if (result == ERROR_SUCCESS)
	{
		controllerValid = true;
	}
	else
	{
		controllerValid = false;
	}

#	ifdef _ENGINE_DEBUG
	controllerInputUpdated = true;
#	endif
}
const XINPUT_STATE& CXBoxController::GetState()
{
#	ifdef _ENGINE_DEBUG
	if (!controllerInputUpdated)
	{
		throw core::MissingDataException();
	}
#	endif
    return controllerState;
}
bool CXBoxController::IsConnected()
{
#	ifdef _ENGINE_DEBUG
	if (!controllerInputUpdated)
	{
		throw core::MissingDataException();
	}
#	endif
    return controllerValid;
}

void CXBoxController::Vibrate(int leftVal, int rightVal)
{
    // Create a Vibraton State
    XINPUT_VIBRATION vibration;

    // Zeroise the Vibration
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

    // Set the Vibration Values
	vibration.wLeftMotorSpeed	= leftVal;
	vibration.wRightMotorSpeed	= rightVal;

    // Vibrate the controller
    XInputSetState(controllerIndex, &vibration);
}

//	GetAnalog( axis_group ) : Returns the analog input of given group.
// Inputs are normalized to the -1 to +1 range.
// Triggers are normally only in the 0 to +1 range.
Vector3d CXBoxController::GetAnalog ( const arAnalogIndex analogIndex )
{
	if ( analogIndex == kAnalogIndexLeftStick )
	{
		Vector2d controllerInput;
		Real controllerStickMagnitude;

		controllerInput = Vector2d ( GetState().Gamepad.sThumbLX, GetState().Gamepad.sThumbLY );
		controllerStickMagnitude = controllerInput.magnitude();
		// If within deadzone, zero out the input
		if ( controllerStickMagnitude < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE )
		{
			controllerInput = Vector2d(0,0);
		}
		// Else, we normalize the input.
		else
		{
			// Need new value to normalize against:
			controllerStickMagnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			controllerStickMagnitude /= (32768.0F - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			// And we normalize against that.
			controllerInput = controllerInput.normal() * controllerStickMagnitude;
		}

		return controllerInput;
	}
	else if ( analogIndex == kAnalogIndexRightStick )
	{
		Vector2d controllerInput;
		Real controllerStickMagnitude;

		controllerInput = Vector2d ( GetState().Gamepad.sThumbRX, GetState().Gamepad.sThumbRY );
		controllerStickMagnitude = controllerInput.magnitude();
		// If within deadzone, zero out the input
		if ( controllerStickMagnitude < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE )
		{
			controllerInput = Vector2d(0,0);
		}
		// Else, we normalize the input.
		else
		{
			// Need new value to normalize against:
			controllerStickMagnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			controllerStickMagnitude /= (32768.0F - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			// And we normalize against that.
			controllerInput = controllerInput.normal() * controllerStickMagnitude;
		}

		return controllerInput;
	}
	else if ( analogIndex == kAnalogIndexTriggers )
	{
		Vector2d controllerInput;

		controllerInput.x = (Real)(GetState().Gamepad.bLeftTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		controllerInput.x /= 255.0F - XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		controllerInput.y = (Real)(GetState().Gamepad.bRightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		controllerInput.y /= 255.0F - XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

		return controllerInput;
	}

	return Vector3f(0,0,0);
}