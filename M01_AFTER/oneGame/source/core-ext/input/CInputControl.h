// Lovely interface to toss control here and there and everywhere
// Creates a layer between input system 


#ifndef _C_INPUT_CONTROL_H_
#define _C_INPUT_CONTROL_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Vector3d.h"
#include <list>

class CEmulatedInputControl;

class CInputControl
{

public:
	CORE_API explicit		CInputControl ( void* owner );
	CORE_API 				~CInputControl ( void );

	CORE_API void			Update ( void* owner, float deltaTime );
	CORE_API void			ZeroValues ( void* owner );

	CORE_API void			Capture ( void );
	CORE_API void			Release ( void );
	CORE_API bool			HasCapture ( void ) { return m_inputs.front()==this; };

	CORE_API static CInputControl*	GetActive ( void ) { return m_inputs.front(); };
	CORE_API void*			GetUser ( void ) { return current_user; };

public:
	struct ControlValue {	// Control value class
	public:
		operator const bool() const { return (Value >= 0.5f); }
		bool		pressed ( void ) { return _pressed; }
		bool		released ( void ) { return _released; }
		ftype Value;
		ftype PreviousValue;
		void		Zero ( void ) {
			_pressed = false;
			_released = false;
			Value = PreviousValue;
		}
		void		Skip ( void ) {
			_pressed = false;
			_released = false;
			PreviousValue = Value;
		}

		// Constructor
		/*ControlValue ( void )
			: Value(0), PreviousValue(0), _pressed(false), _released(false)
		{
			;
		}*/
	private:
		friend		CInputControl;
		friend		CEmulatedInputControl;
		void		Update( ftype NewValue ) {
			_pressed = ((!_pressed) && ((NewValue >= 0.5f)&&(PreviousValue < 0.5f)));
			_released = ((!_released) && ((NewValue <= 0.5f)&&(PreviousValue > 0.5f)));
			PreviousValue = Value;
			Value = NewValue;
		}
		bool		_pressed;
		bool		_released;
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

			ControlValue menuToggle;
			ControlValue menuInventory;
			ControlValue menuLogbook;
			ControlValue menuCharscreen;
			ControlValue menuSkills;
			ControlValue menuQuestlog;
			ControlValue menuCrafting;

			ControlValue tglCompanion;
			ControlValue tglContext;
			ControlValue tglEyewear;
		};
		ControlValue _total [19];
	} axes_t;

	axes_t axes;

	static const short max_axes	= 19;

	/*float		fInput [10];
	float		fInputPrev [10];

	enum eInputIndex
	{
		iCrouch		= 0,
		iJump,
		iPrimary,
		iSecondary,
		iTab,
		iSprint,
		iProne,
		iThrow
	};
	static const short maxInputs	= 8;*/
	
private:
	void* current_user;
	static std::list<CInputControl*>	m_inputs;
	int	zerotimer;
};


#endif//_C_INPUT_CONTROL_H_