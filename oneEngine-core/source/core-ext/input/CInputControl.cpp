
#include "CInputControl.h"

#include "core/input/CInput.h"
#include "core/input/CXboxController.h"

#include "core/settings/CGameSettings.h"

#include "core/math/Vector2d.h"
#include "core/math/Math.h"

#include <algorithm>


std::list<CInputControl*>	CInputControl::m_inputs;

CInputControl::CInputControl ( void* owner )
{
	current_user = owner;

	m_inputs.push_back( this );

	// Initialize control to zero
	for ( short i = 0; i < max_axes; ++i ) {
		axes._total[i].PreviousValue = 0;
		axes._total[i].Value = 0;
		axes._total[i]._pressed = false;
		axes._total[i]._released = false;
	}

	zerotimer = 1;
}
CInputControl::~CInputControl ( void )
{
	// Remove self from input control list
	std::list<CInputControl*>::iterator position;
	position = std::find( m_inputs.begin(), m_inputs.end(), this );
	if ( position != m_inputs.end() )
	{
		m_inputs.erase( position );
	}
}

void CInputControl::Capture ( void )
{
	// Remove self from input control list
	std::list<CInputControl*>::iterator position;
	position = std::find( m_inputs.begin(), m_inputs.end(), this );
	if ( position != m_inputs.end() )
	{
		m_inputs.erase( position );
	}
	// Add self to front
	m_inputs.push_front( this );
}
void CInputControl::Release ( void )
{
	bool wasTop = (this == m_inputs.front());
	// Remove self from input control list
	std::list<CInputControl*>::iterator position;
	position = std::find( m_inputs.begin(), m_inputs.end(), this );
	if ( position != m_inputs.end() )
	{
		m_inputs.erase( position );
	}
	// Add self to back
	m_inputs.push_back( this );

	// Refresh the top level's input
	if ( wasTop ) {
		m_inputs.front()->ZeroValues( m_inputs.front()->current_user );
	}
}

void CInputControl::Update ( void* owner, float deltaTime )
{
	// Get inputs from style
	if ( CGameSettings::Active()->i_cl_KeyboardStyle == 0 )
	{
		// Get directional input
		// First clip controller input
		Vector2d controllerInput;
		Real controllerStickMagnitude;
		controllerInput = Vector2d ( Input::xboxControl->GetState().Gamepad.sThumbLY, Input::xboxControl->GetState().Gamepad.sThumbLX );
		controllerStickMagnitude = controllerInput.magnitude();
		if ( controllerStickMagnitude < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ) {
			controllerInput = Vector2d(0,0);
		}
		else {
			controllerStickMagnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
			controllerStickMagnitude /= (32768.0-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			controllerInput = controllerInput.normal() * controllerStickMagnitude;
		}
		// Now perform the rest of the inputs
		vDirInput.x		= (float)Input::Key('W') - (float)Input::Key('S');
			vDirInput.x+= controllerInput.x;
		vDirInput.y		= (float)Input::Key('A') - (float)Input::Key('D');
			vDirInput.y-= controllerInput.y;
		vDirInput.z		= (float)Input::Key(Keys.Space) - (float)Input::Key(Keys.Control) - (float)Input::Key('C');
			vDirInput.z+= ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)?1:0);
			vDirInput.z-= ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)?1:0);
		// Get turnint input
		// First clicp controller input
		controllerInput = Vector2d ( Input::xboxControl->GetState().Gamepad.sThumbRX, Input::xboxControl->GetState().Gamepad.sThumbRY );
		controllerStickMagnitude = controllerInput.magnitude();
		if ( controllerStickMagnitude < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ) {
			controllerInput = Vector2d(0,0);
		}
		else {
			controllerStickMagnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
			controllerStickMagnitude /= (32768.0-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			controllerInput = controllerInput.normal() * controllerStickMagnitude;
		}
		// Now perform the rest of the inputs
		vMouseInput.x	= (float)CInput::DeltaMouseX()*0.5f;
			vMouseInput.x+= (controllerInput.x * deltaTime) * 160.0f;
		vMouseInput.y	= (float)CInput::DeltaMouseY()*0.5f;
			vMouseInput.y-= (controllerInput.y * deltaTime) * 160.0f;
			vMouseInput.z	= (float)CInput::DeltaMouseW();
			vMouseInput.z+= ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)?1:0);
			vMouseInput.z-= ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)?1:0);
			
		// Get the keydown input
		//axes.crouch.Update			( (float)Input::Key(Keys.Control) + (float)Input::Key('C') );
		axes.crouch.Update			( -Math.Clamp(vDirInput.z,-1,0) );
		//axes.jump.Update			( (float)Input::Key(Keys.Space) );
		axes.jump.Update			( Math.Clamp(vDirInput.z,0,1) );
		axes.primary.Update			( (float)CInput::Mouse(CInput::MBLeft) + ((Input::xboxControl->GetState().Gamepad.bRightTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD)/(255.0f-XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) );
		axes.secondary.Update		( (float)CInput::Mouse(CInput::MBRight) + ((Input::xboxControl->GetState().Gamepad.bLeftTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD)/(255.0f-XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) );
		//axes.sprint.Update			( (float)Input::Key(Keys.Shift) );
		axes.sprint.Update			( (float)Input::Key(Keys.Shift) + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)?1:0) );
		axes.prone.Update			( (float)Input::Keydown('Z') );
		//axes.defend.Update			( (float)Input::Key('Q') );
		axes.defend.Update			( (float)Input::Key('Q') + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)?1:0) );
		//axes.use.Update				( (float)Input::Key('E') );
		axes.use.Update				( (float)Input::Key('E') + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)?1:0) );
		//axes.tertiary.Update		( (float)Input::Key('R') );
		axes.tertiary.Update		( (float)Input::Key('R') + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)?1:0) );

		axes.menuToggle.Update		( (float)Input::Key(Keys.Tab) + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)?1:0) );
		axes.menuInventory.Update	( (float)Input::Key('I') + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)?1:0) );
		axes.menuLogbook.Update		( (float)Input::Key('L') );
		axes.menuCharscreen.Update	( (float)Input::Key('O') );
		axes.menuSkills.Update		( (float)Input::Key('K') );
		axes.menuQuestlog.Update	( (float)Input::Key('J') );
		axes.menuCrafting.Update	( (float)Input::Key('U') );

		axes.tglCompanion.Update	( (float)Input::Key('F') + ((Input::xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)?1:0) );
		axes.tglContext.Update		( (float)Input::Key('X') );
		axes.tglEyewear.Update		( (float)Input::Key('V') );
	}
	else if ( CGameSettings::Active()->i_cl_KeyboardStyle == 1 ) // US DVORAK
	{
		// Get directional input
		vDirInput.x		= (float)Input::Key(',') - (float)Input::Key('O');
		vDirInput.y		= (float)Input::Key('A') - (float)Input::Key('E');
		vDirInput.z		= (float)Input::Key(Keys.Space) - (float)Input::Key(Keys.Control) - (float)Input::Key('C');
		// Get turnint input
		vMouseInput.x	= (float)CInput::DeltaMouseX();
		vMouseInput.y	= (float)CInput::DeltaMouseY();
		vMouseInput.z	= (float)CInput::DeltaMouseW();

		// Get the keydown input
		axes.crouch.Update			( (float)Input::Key(Keys.Control) + (float)Input::Key('J') );
		axes.jump.Update			( (float)Input::Key(Keys.Space) );
		axes.primary.Update			( (float)CInput::Mouse(CInput::MBLeft) );
		axes.secondary.Update		( (float)CInput::Mouse(CInput::MBRight) );
		axes.sprint.Update			( (float)Input::Key(Keys.Shift) );
		axes.prone.Update			( (float)Input::Keydown(';') );
		axes.defend.Update			( (float)Input::Key('\'') );
		axes.use.Update				( (float)Input::Key('.') );
		axes.tertiary.Update		( (float)Input::Key('P') );

		axes.menuToggle.Update		( (float)Input::Key(Keys.Tab) );
		axes.menuInventory.Update	( (float)Input::Key('C') );
		axes.menuLogbook.Update		( (float)Input::Key('N') );
		axes.menuCharscreen.Update	( (float)Input::Key('R') );
		axes.menuSkills.Update		( (float)Input::Key('T') );
		axes.menuQuestlog.Update	( (float)Input::Key('H') );
		axes.menuCrafting.Update	( (float)Input::Key('G') );

		axes.tglCompanion.Update	( (float)Input::Key('U') );
		axes.tglContext.Update		( (float)Input::Key('Q') );
		axes.tglEyewear.Update		( (float)Input::Key('K') );
	}

	/*
	// Get directional input
	vDirInput.x		= (float)CInput::key['W'] - (float)CInput::key['S'];
	vDirInput.y		= (float)CInput::key['A'] - (float)CInput::key['D'];
	vDirInput.z		= (float)CInput::key[VK_SPACE];
	// Get turnint input
	vTurnInput.x	= (float)CInput::deltaMouseX;
	vTurnInput.y	= (float)CInput::deltaMouseY;
	vTurnInput.z	= (float)CInput::deltaMouseW;
	// Get the keydown input
	for ( short i = 0; i < maxInputs; i += 1 )
	{
		fInputPrev[ i ] = fInput[ i ];
	}
	fInput[ iCrouch ]	= (float)CInput::key[VK_CONTROL] + (float)CInput::key['C'];
	fInput[ iJump ]		= (float)CInput::key[VK_SPACE];
	fInput[ iPrimary ]	= (float)CInput::mouse[CInput::MBLeft];
	fInput[ iSecondary ]= (float)CInput::mouse[CInput::MBRight];
	fInput[ iTab ]		= (float)CInput::key[VK_TAB];
	fInput[ iSprint ]	= (float)CInput::key[VK_SHIFT];
	fInput[ iProne ]	= (float)CInput::keydown['Z'];
	fInput[ iThrow ]	= (float)CInput::key['Q'];
	*/

	// Check for owning of control events
	if ( m_inputs.front() != this )
	{
		ZeroValues( current_user );
		zerotimer = 3;
	}
	else if ( zerotimer > 0 ) // Check for delaying input when swapping
	{
		ZeroValues( current_user );
		zerotimer -= 1;
	}
}

void CInputControl::ZeroValues ( void* owner )
{
	// Get directional input
	vDirInput.x		= 0.0f;
	vDirInput.y		= 0.0f;
	vDirInput.z		= 0.0f;
	// Get turnint input
	vMouseInput.x	= 0.0f;
	vMouseInput.y	= 0.0f;
	vMouseInput.z	= 0.0f;

	// Get the keydown input
	axes.crouch.Zero();
	axes.jump.Zero();
	axes.primary.Zero();
	axes.secondary.Zero();
	axes.sprint.Zero();
	axes.prone.Zero();
	axes.defend.Zero();
	axes.use.Zero();
	axes.tertiary.Zero();

	axes.menuToggle.Zero();
	axes.menuInventory.Zero();
	axes.menuLogbook.Zero();
	axes.menuCharscreen.Zero();
	axes.menuSkills.Zero();

	axes.tglCompanion.Zero();
}