
// Include stuph
#include "../CDuskGUI.h"
#include "CDuskGUIVector3fPicker.h"
#include "core/system/Screen.h" // Include screen properties
//#include "renderer/system/glMainSystem.h" // Include the main system

// A vector3d picker
CDuskGUI::Handle CDuskGUI::CreateVector3fPicker ( const Handle& parent, const Vector3f& vect )
{
	vElements.push_back( new CDuskGUIVector3fPicker() );
	vElements.back()->parent = parent;
	((CDuskGUIVector3fPicker*)vElements.back())->colorValue = vect;
	return Handle(vElements.size()-1);
}
// Update Vector3f picker
void CDuskGUI::UpdateVector3fPicker ( const Handle& handle, Vector3f & inOutVectorVal )
{
	CDuskGUIVector3fPicker* cp = (CDuskGUIVector3fPicker*)vElements[int(handle)];
	if ( cp->hVecX == -1 )
	{
		if (( fabs(cp->lastColorValue.x-inOutVectorVal.x) > FTYPE_PRECISION )
		  ||( fabs(cp->lastColorValue.y-inOutVectorVal.y) > FTYPE_PRECISION )
		  ||( fabs(cp->lastColorValue.z-inOutVectorVal.z) > FTYPE_PRECISION ))
		{
			cp->colorValue = inOutVectorVal;
		}
		else
		{
			inOutVectorVal = cp->colorValue;
		}
	}
	else
	{
		UpdateFloatfield( cp->hVecX, inOutVectorVal.x );
		UpdateFloatfield( cp->hVecY, inOutVectorVal.y );
		UpdateFloatfield( cp->hVecZ, inOutVectorVal.z );

		cp->colorValue = inOutVectorVal;

		if ( !VALID_FLOAT(inOutVectorVal.x) ) {
			inOutVectorVal.x = 0;
			cp->colorValue.y = 0;
		}
		if ( !VALID_FLOAT(inOutVectorVal.y) ) {
			inOutVectorVal.y = 0;
			cp->colorValue.y = 0;
		}
		if ( !VALID_FLOAT(inOutVectorVal.z) ) {
			inOutVectorVal.z = 0;
			cp->colorValue.z = 0;
		}
	}
}
void CDuskGUI::SetVector3fPicker ( const Handle & handle, Vector3f & inVectorVal )
{
	CDuskGUIVector3fPicker* cp = (CDuskGUIVector3fPicker*)vElements[int(handle)];
	cp->SetVector( inVectorVal );
}

CDuskGUIVector3fPicker::CDuskGUIVector3fPicker ( void )
	: CDuskGUIButton(), inDialogueMode(false), homeRect(rect)
{
	/*hVecX = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.x );
	hVecY = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.y );
	hVecZ = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.z );*/
	hVecX = -1;
}
CDuskGUIVector3fPicker::~CDuskGUIVector3fPicker ( void )
{
	activeGUI->DeleteElement( hVecX );
	activeGUI->DeleteElement( hVecY );
	activeGUI->DeleteElement( hVecZ );
}

void CDuskGUIVector3fPicker::SetVector ( Vector3f& v )
{
	lastColorValue = v;
	colorValue = v;
}

void CDuskGUIVector3fPicker::Update ( void )
{
	if ( hVecX == -1 )
	{
		hVecX = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.x );
		hVecY = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.y );
		hVecZ = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.z );

		/*activeGUI->SetElementRect( hVecX, Rect( rect.size.x*0.1f, rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );
		activeGUI->SetElementRect( hVecY, Rect( rect.size.x*0.4f, rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );
		activeGUI->SetElementRect( hVecZ, Rect( rect.size.x*0.7f, rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );*/
	}
	lastColorValue = colorValue;

	activeGUI->UpdateFloatfield( hVecX, colorValue.x );
	activeGUI->UpdateFloatfield( hVecY, colorValue.y );
	activeGUI->UpdateFloatfield( hVecZ, colorValue.z );

	CDuskGUIButton::Update();

	if ( (!visible) || (!drawn) )
		return;

	//if ( !inDialogueMode )
	//{
		// Button state
		homeRect = rect;

		// TODO: y position incorrect
		activeGUI->SetElementRect( hVecX, Rect( rect.size.x*0.1f, rect.size.y*1.4f + rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );
		activeGUI->SetElementRect( hVecY, Rect( rect.size.x*0.4f, rect.size.y*1.4f + rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );
		activeGUI->SetElementRect( hVecZ, Rect( rect.size.x*0.7f, rect.size.y*1.4f + rect.size.y*0.05f, rect.size.x * 0.25f, rect.size.y * 0.9f ) );
	/*}
	else
	{
		// Color picking state
		activeGUI->hCurrentDialogue = activeGUI->hCurrentElement;

		activeGUI->SetElementRect( hVecX, Rect( rect.size.x*0.1f, rect.size.y*0.2f, rect.size.x * 0.3f, rect.size.y * 0.17f ) );
		activeGUI->SetElementRect( hVecY, Rect( rect.size.x*0.1f, rect.size.y*0.4f, rect.size.x * 0.3f, rect.size.y * 0.17f ) );
		activeGUI->SetElementRect( hVecZ, Rect( rect.size.x*0.1f, rect.size.y*0.6f, rect.size.x * 0.3f, rect.size.y * 0.17f ) );

		if ( mouseIn )
		{
		
		}
		else
		{
			if ( core::Input::MouseDown(core::kMBLeft) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}
			else if ( core::Input::MouseDown(core::kMBRight) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}
		}
	}*/
}

void CDuskGUIVector3fPicker::Render ( void )
{
	Rect temp = rect;
	rect = homeRect;
		drawRect( rect );
		CDuskGUIButton::Render();
		setDrawDefault();
		drawRectWire( rect );
	rect = temp;

	if ( inDialogueMode )
	{
		drawRectWire( rect );
		drawRect( rect );
	}
}