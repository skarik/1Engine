
// Include stuph
#include "../CDuskGUI.h"
#include "CDuskGUIVector3dPicker.h"
#include "core/system/Screen.h" // Include screen properties
//#include "renderer/system/glMainSystem.h" // Include the main system

// A vector3d picker
CDuskGUI::Handle CDuskGUI::CreateVector3dPicker ( const Handle& parent, const Vector3d& vect )
{
	vElements.push_back( new CDuskGUIVector3dPicker() );
	vElements.back()->parent = parent;
	((CDuskGUIVector3dPicker*)vElements.back())->colorValue = vect;
	return Handle(vElements.size()-1);
}
// Update Vector3d picker
void CDuskGUI::UpdateVector3dPicker ( const Handle& handle, Vector3d & inOutVectorVal )
{
	CDuskGUIVector3dPicker* cp = (CDuskGUIVector3dPicker*)vElements[int(handle)];
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
void CDuskGUI::SetVector3dPicker ( const Handle & handle, Vector3d & inVectorVal )
{
	CDuskGUIVector3dPicker* cp = (CDuskGUIVector3dPicker*)vElements[int(handle)];
	cp->SetVector( inVectorVal );
}

CDuskGUIVector3dPicker::CDuskGUIVector3dPicker ( void )
	: CDuskGUIButton(), inDialogueMode(false), homeRect(rect)
{
	/*hVecX = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.x );
	hVecY = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.y );
	hVecZ = activeGUI->CreateFloatfield( activeGUI->GetFromPointer( this ), colorValue.z );*/
	hVecX = -1;
}
CDuskGUIVector3dPicker::~CDuskGUIVector3dPicker ( void )
{
	activeGUI->DeleteElement( hVecX );
	activeGUI->DeleteElement( hVecY );
	activeGUI->DeleteElement( hVecZ );
}

void CDuskGUIVector3dPicker::SetVector ( Vector3d& v )
{
	lastColorValue = v;
	colorValue = v;
}

void CDuskGUIVector3dPicker::Update ( void )
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
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}
			else if ( CInput::MouseDown(CInput::MBRight) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}
		}
	}*/
}

void CDuskGUIVector3dPicker::Render ( void )
{
	Rect temp = rect;
	rect = homeRect;
		/*activeGUI->matDefault->bind();
			//glColor4f( colorValue.red,colorValue.green,colorValue.blue,colorValue.alpha );
			glColor4f(
				activeGUI->matDefault->diffuse.red * 0.6f,
				activeGUI->matDefault->diffuse.green * 0.6f,
				activeGUI->matDefault->diffuse.blue * 0.6f,
				activeGUI->matDefault->diffuse.alpha * 0.6f );
			GLd.DrawSet2DMode( GL.D2D_FLAT );
			GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );
		activeGUI->matDefault->unbind();*/
		drawRect( rect );
		CDuskGUIButton::Render();
		setDrawDefault();
		drawRectWire( rect );
		/*activeGUI->matDefault->bind();
			//glColor4f( colorValue.red,colorValue.green,colorValue.blue,1.0f );
			glColor4f(
				activeGUI->matDefault->diffuse.red,
				activeGUI->matDefault->diffuse.green,
				activeGUI->matDefault->diffuse.blue,
				activeGUI->matDefault->diffuse.alpha * 0.6f );
			GLd.DrawSet2DMode( GL.D2D_WIRE );
			GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );
		activeGUI->matDefault->unbind();*/
	rect = temp;

	if ( inDialogueMode )
	{
		//activeGUI->matDefault->bind();

		/*glColor4f(
			activeGUI->matDefault->diffuse.red * 0.6f,
			activeGUI->matDefault->diffuse.green * 0.6f,
			activeGUI->matDefault->diffuse.blue * 0.6f,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_WIRE );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );*/
		drawRectWire( rect );

		/*glColor4f(
			activeGUI->matDefault->diffuse.red,
			activeGUI->matDefault->diffuse.green,
			activeGUI->matDefault->diffuse.blue,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_FLAT );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );*/
		drawRect( rect );
	}
}