
#include "../CDuskGUI.h"
#include "CDuskGUISlider.h"
#include "core/math/Math.h"

// Button used for clicking and stuff
CDuskGUI::Handle CDuskGUI::CreateSlider	( const Handle& parent )
{
	vElements.push_back( new CDuskGUISlider() );
	vElements.back()->parent = parent;
	//((CDuskGUIButton*)vElements.back())->isPressed = false;
	return Handle(vElements.size()-1);
}
// One-way get
ftype CDuskGUI::GetSliderValue ( const Handle& handle )
{
	CDuskGUISlider* slider = (CDuskGUISlider*)vElements[int(handle)];
	return slider->value.mCurVal;
}
// Slider updating
void CDuskGUI::UpdateSlider ( const Handle& handle, ftype & inOutSliderVal )
{
	CDuskGUISlider* slider = (CDuskGUISlider*)vElements[int(handle)];
	if ( slider->prevValue.mCurVal != inOutSliderVal )
	{
		slider->value.SetValue( inOutSliderVal );
	}
	else
	{
		inOutSliderVal = slider->value.mCurVal;
	}
}
// Slider options
void CDuskGUI::SetSliderMinMax ( const Handle& handle, const ftype newmin, const ftype newmax )
{
	CDuskGUISlider* slider = (CDuskGUISlider*)vElements[int(handle)];
	slider->value.SetRange( newmin, newmax );
	slider->prevValue.SetRange( newmin, newmax );
}
void CDuskGUI::SetSliderSnapping ( const Handle& handle, const ftype newdiv )
{
	CDuskGUISlider* slider = (CDuskGUISlider*)vElements[int(handle)];
	slider->divs = newdiv;
}


// === Slider Object Definition ===
CDuskGUISlider::CDuskGUISlider ( void ) : CDuskGUIPanel(17)
{
	divs = -1;
	value.mMinVal = 0.0f;
	value.mMaxVal = 1.0f;
	value.mCurVal = 0.5f;
	prevValue = value;

	isDragging = false;
}

// Overridable update
void CDuskGUISlider::Update ( void )
{
	CDuskGUIElement::Update();

	prevValue = value;
	if ( mouseIn )
	{
		// get percent where mouse is maybe?

		// or if mouse in, start dragging.
		// based on mouse change of X, change percent.
		if ( CInput::MouseDown(CInput::MBLeft) )
		{
			isDragging = true;
		}
		if ( isDragging ) {
			if ( CInput::MouseUp(CInput::MBLeft) )
			{
				isDragging = false;
			}
		}

		if ( isDragging )
		{
			Rect screen = activeGUI->GetScreenRect();
			ftype valPercent = (value.mCurVal-value.mMinVal)/(value.mMaxVal-value.mMinVal); 
			Vector2d vDeltaPoint = Vector2d( Input::DeltaMouseX()/(ftype)Screen::Info.width * screen.size.x, Input::DeltaMouseY()/(ftype)Screen::Info.height * screen.size.y );

			valPercent += ( vDeltaPoint.x / rect.size.x ) * 1.04f;
			valPercent = Math::saturate(valPercent);

			value.SetValue( value.mMinVal + ((value.mMaxVal-value.mMinVal)*valPercent) );
			//value.mCurVal = ( value.mMinVal + ((value.mMaxVal-value.mMinVal)*valPercent) );
		}
	}
	else
	{
		isDragging = false;
	}
	
}
void CDuskGUISlider::Render ( void )
{
	// Material binding
	if ( mouseIn )
	{
		if ( isDragging )
			setDrawDown();
		else
			setDrawHover();
	}
	else {
		setDrawDefault();
	}

	ftype valPercent = (value.mCurVal-value.mMinVal)/(value.mMaxVal-value.mMinVal);

	// Begin draw/ material

		// draw slide area
		drawRectWire( Rect( rect.pos.x, rect.pos.y+rect.size.y*0.45f, rect.size.x, rect.size.y*0.05f ) );
		drawRect( Rect( rect.pos.x, rect.pos.y+rect.size.y*0.50f, rect.size.x, rect.size.y*0.05f ) );

		if ( hasFocus ) {
			setSubdrawSelection();
		}
		// draw slider
		/*glColor4f(
			!hasFocus ? glMaterial::current->diffuse.red * 0.6f : glMaterial::current->diffuse.red * 1.1f,
			!hasFocus ? glMaterial::current->diffuse.green * 0.6f : glMaterial::current->diffuse.green * 1.1f,
			!hasFocus ? glMaterial::current->diffuse.blue * 0.6f : glMaterial::current->diffuse.blue * 1.1f,
			glMaterial::current->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_WIRE );*/
		drawRectWire( Rect( rect.pos.x + rect.size.x*(valPercent-0.03f), rect.pos.y, rect.size.x*0.06f, rect.size.y ) );

		setSubdrawDefault();
		drawRect( Rect( rect.pos.x + rect.size.x*(valPercent-0.03f), rect.pos.y, rect.size.x*0.06f, rect.size.y ) );

	// Now draw text
		
	//GL.DrawAutoText( rect.pos.x + rect.size.x*0.1f, rect.pos.y + rect.size.y*0.1f  + 0.02f, label.c_str() );
	char tempstr [512];
	sprintf( tempstr, "%.2lf", value.mCurVal );
	drawTextCentered( rect.pos.x + rect.size.x*valPercent, rect.pos.y + rect.size.y*0.7f, tempstr );
}