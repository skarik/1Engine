#include "CInputControl.h"

#include "core/input/CInput.h"
#include "core/input/CXboxController.h"

#include "core/settings/CGameSettings.h"

#include "core/math/Vector2.h"
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
	// Update xbox controller:
	InputControl::CXBoxController* xboxControl = nullptr; // TODO: InputControl::CXBoxController::GetController(0)
	xboxControl->UpdateState();

	// Get inputs from style
	if ( CGameSettings::Active()->i_cl_KeyboardStyle == 0 )
	{
		Vector2f controllerInput;

		// Get directional input
		// First clip controller input
		controllerInput = xboxControl->GetAnalog(InputControl::kAnalogIndexLeftStick);
		// Now perform the rest of the inputs
		vDirInput.x		= (float)core::Input::Key('D') - (float)core::Input::Key('A');
			vDirInput.x+= controllerInput.x;
		vDirInput.y		= (float)core::Input::Key('W') - (float)core::Input::Key('S');
			vDirInput.y+= controllerInput.y;
		vDirInput.z		= (float)core::Input::Key(core::kVkSpace) - (float)core::Input::Key(core::kVkControl) - (float)core::Input::Key('C');
			vDirInput.z+= ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)?1:0);
			vDirInput.z-= ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)?1:0);
		
		// Get turning input
		// First clicp controller input
		controllerInput = xboxControl->GetAnalog(InputControl::kAnalogIndexRightStick);
		// Now perform the rest of the inputs
		vMouseInput.x	= (float)core::Input::DeltaMouseX()*0.5f;
			vMouseInput.x+= (controllerInput.x * deltaTime) * 160.0f;
		vMouseInput.y	= (float)core::Input::DeltaMouseY()*0.5f;
			vMouseInput.y-= (controllerInput.y * deltaTime) * 160.0f;
			vMouseInput.z	= (float)core::Input::DeltaMouseScroll();
			vMouseInput.z+= ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)?1:0);
			vMouseInput.z-= ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)?1:0);
			
		// Get the keydown input
		controllerInput = xboxControl->GetAnalog(InputControl::kAnalogIndexTriggers);
		//axes.crouch.Update			( (float)core::Input::Key(core::kVkControl) + (float)core::Input::Key('C') );
		axes.crouch.Update			( -math::clamp<Real>(vDirInput.z,-1,0) );
		//axes.jump.Update			( (float)core::Input::Key(core::kVkSpace) );
		axes.jump.Update			( math::clamp<Real>(vDirInput.z,0,1) );
		axes.primary.Update			( (float)core::Input::Mouse(core::kMBLeft)  + controllerInput.y );
		axes.secondary.Update		( (float)core::Input::Mouse(core::kMBRight) + controllerInput.x );
		//axes.sprint.Update			( (float)core::Input::Key(core::kVkShift) );
		axes.sprint.Update			( (float)core::Input::Key(core::kVkShift) + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)?1:0) );
		axes.prone.Update			( (float)core::Input::Keydown('Z') );
		//axes.defend.Update			( (float)core::Input::Key('Q') );
		axes.defend.Update			( (float)core::Input::Key('Q') + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)?1:0) );
		//axes.use.Update				( (float)core::Input::Key('E') );
		axes.use.Update				( (float)core::Input::Key('E') + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)?1:0) );
		//axes.tertiary.Update		( (float)core::Input::Key('R') );
		axes.tertiary.Update		( (float)core::Input::Key('R') + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)?1:0) );

		axes.menuToggle.Update		( (float)core::Input::Key(core::kVkTab) + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)?1:0) );
		axes.menuInventory.Update	( (float)core::Input::Key('I') + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)?1:0) );
		axes.menuLogbook.Update		( (float)core::Input::Key('L') );
		axes.menuCharscreen.Update	( (float)core::Input::Key('O') );
		axes.menuSkills.Update		( (float)core::Input::Key('K') );
		axes.menuQuestlog.Update	( (float)core::Input::Key('J') );
		axes.menuCrafting.Update	( (float)core::Input::Key('U') );

		axes.tglCompanion.Update	( (float)core::Input::Key('F') + ((xboxControl->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)?1:0) );
		axes.tglContext.Update		( (float)core::Input::Key('X') );
		axes.tglEyewear.Update		( (float)core::Input::Key('V') );
	}
	else if ( CGameSettings::Active()->i_cl_KeyboardStyle == 1 ) // US DVORAK
	{
		// Get directional input
		vDirInput.x		= (float)core::Input::Key(',') - (float)core::Input::Key('O');
		vDirInput.y		= (float)core::Input::Key('A') - (float)core::Input::Key('E');
		vDirInput.z		= (float)core::Input::Key(core::kVkSpace) - (float)core::Input::Key(core::kVkControl) - (float)core::Input::Key('C');
		// Get turnint input
		vMouseInput.x	= (float)core::Input::DeltaMouseX();
		vMouseInput.y	= (float)core::Input::DeltaMouseY();
		vMouseInput.z	= (float)core::Input::DeltaMouseScroll();

		// Get the keydown input
		axes.crouch.Update			( (float)core::Input::Key(core::kVkControl) + (float)core::Input::Key('J') );
		axes.jump.Update			( (float)core::Input::Key(core::kVkSpace) );
		axes.primary.Update			( (float)core::Input::Mouse(core::kMBLeft) );
		axes.secondary.Update		( (float)core::Input::Mouse(core::kMBRight) );
		axes.sprint.Update			( (float)core::Input::Key(core::kVkShift) );
		axes.prone.Update			( (float)core::Input::Keydown(';') );
		axes.defend.Update			( (float)core::Input::Key('\'') );
		axes.use.Update				( (float)core::Input::Key('.') );
		axes.tertiary.Update		( (float)core::Input::Key('P') );

		axes.menuToggle.Update		( (float)core::Input::Key(core::kVkTab) );
		axes.menuInventory.Update	( (float)core::Input::Key('C') );
		axes.menuLogbook.Update		( (float)core::Input::Key('N') );
		axes.menuCharscreen.Update	( (float)core::Input::Key('R') );
		axes.menuSkills.Update		( (float)core::Input::Key('T') );
		axes.menuQuestlog.Update	( (float)core::Input::Key('H') );
		axes.menuCrafting.Update	( (float)core::Input::Key('G') );

		axes.tglCompanion.Update	( (float)core::Input::Key('U') );
		axes.tglContext.Update		( (float)core::Input::Key('Q') );
		axes.tglEyewear.Update		( (float)core::Input::Key('K') );
	}

	/*
	// Get directional input
	vDirInput.x		= (float)core::Input::key['W'] - (float)core::Input::key['S'];
	vDirInput.y		= (float)core::Input::key['A'] - (float)core::Input::key['D'];
	vDirInput.z		= (float)core::Input::key[VK_SPACE];
	// Get turnint input
	vTurnInput.x	= (float)core::Input::deltaMouseX;
	vTurnInput.y	= (float)core::Input::deltaMouseY;
	vTurnInput.z	= (float)core::Input::deltaMouseW;
	// Get the keydown input
	for ( short i = 0; i < maxInputs; i += 1 )
	{
		fInputPrev[ i ] = fInput[ i ];
	}
	fInput[ iCrouch ]	= (float)core::Input::key[VK_CONTROL] + (float)core::Input::key['C'];
	fInput[ iJump ]		= (float)core::Input::key[VK_SPACE];
	fInput[ iPrimary ]	= (float)core::Input::mouse[core::kMBLeft];
	fInput[ iSecondary ]= (float)core::Input::mouse[core::kMBRight];
	fInput[ iTab ]		= (float)core::Input::key[VK_TAB];
	fInput[ iSprint ]	= (float)core::Input::key[VK_SHIFT];
	fInput[ iProne ]	= (float)core::Input::keydown['Z'];
	fInput[ iThrow ]	= (float)core::Input::key['Q'];
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