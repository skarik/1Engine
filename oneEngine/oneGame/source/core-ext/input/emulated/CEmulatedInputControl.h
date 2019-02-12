//===============================================================================================//
//
//		CEmulatedInputControl
//
// Input control that allows for simulation of input.
//
//===============================================================================================//
#ifndef C_EMULATED_INPUT_CONTROL_H_
#define C_EMULATED_INPUT_CONTROL_H_

#include "core-ext/input/CInputControl.h"

class CEmulatedInputControl
{
public:
	explicit CEmulatedInputControl ( void )
	{
		for ( int i = 0; i < kControlInputMAXCOUNT; ++i )
		{
			axes._total[i].Zero();
			keypress_timer[i] = -0.1f;
		}
	}

	void SimulatePress ( const int keyIndex, Real pressLength = 0.1f, bool pressOverride=false ) {
		if ( keypress_timer[keyIndex] <= 0 || pressOverride ) {
			keypress_timer[keyIndex] = pressLength;
		}
	}

	void Update ( float deltatime )
	{
		for ( int i = 0; i < kControlInputMAXCOUNT; ++i )
		{
			axes._total[i].Update( keypress_timer[i] >= 0 );
			if ( keypress_timer[i] >= 0 ) {
				keypress_timer[i] -= deltatime;
			}
		}
	}

public:
	typedef CInputControl::ControlValue ControlValue;

	enum EControlId
	{
		kControlInputCrouch = 0,
		kControlInputJump,
		kControlInputPrimary,
		kControlInputSecondary,
		kControlInputSprint,
		kControlInputProne,
		kControlInputDefend,
		kControlInputUse,
		kControlInputTertiary,

		kControlInputMAXCOUNT
	};

public:
	Vector3f	vDirInput;
	Vector3f	vMouseInput;

	typedef union
	{
		struct {
			ControlValue crouch;
			ControlValue jump;
			ControlValue primary;
			ControlValue secondary;
			ControlValue sprint;
			ControlValue prone;
			ControlValue defend;
			ControlValue use;
			ControlValue tertiary;
		};
		ControlValue _total [kControlInputMAXCOUNT];
	} axes_t;

	axes_t axes;

private:
	
	Real	keypress_timer [kControlInputMAXCOUNT];
};

#endif//C_EMULATED_INPUT_CONTROL_H_