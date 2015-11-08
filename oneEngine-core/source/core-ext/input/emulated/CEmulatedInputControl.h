
#ifndef _C_EMULATED_INPUT_CONTROL_H_
#define _C_EMULATED_INPUT_CONTROL_H_

#include "core/time/time.h"
#include "core-ext/input/CInputControl.h"

class CEmulatedInputControl
{
public:
	explicit CEmulatedInputControl ( void )
	{
		for ( int i = 0; i < MAX_INPUT_COUNT; ++i )
		{
			axes._total[i].Zero();
			keypress_timer[i] = -0.1f;
		}
	}

	void SimulatePress ( const int keyIndex, ftype pressLength = 0.1f, bool pressOverride=false ) {
		if ( keypress_timer[keyIndex] <= 0 || pressOverride ) {
			keypress_timer[keyIndex] = pressLength;
		}
	}

	void Update ( void )
	{
		for ( int i = 0; i < MAX_INPUT_COUNT; ++i )
		{
			axes._total[i].Update( keypress_timer[i] >= 0 );
			if ( keypress_timer[i] >= 0 ) {
				keypress_timer[i] -= Time::deltaTime;
			}
		}
	}

public:
	typedef CInputControl::ControlValue ControlValue;

	enum ControlEnum
	{
		InputCrouch = 0,
		InputJump,
		InputPrimary,
		InputSecondary,
		InputSprint,
		InputProne,
		InputDefend,
		InputUse,
		InputTertiary,

		MAX_INPUT_COUNT
	};

public:
	Vector3d	vDirInput;
	Vector3d	vMouseInput;

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
		ControlValue _total [MAX_INPUT_COUNT];
	} axes_t;

	axes_t axes;

private:
	
	ftype	keypress_timer [MAX_INPUT_COUNT];
};

#endif//_C_EMULATED_INPUT_CONTROL_H_