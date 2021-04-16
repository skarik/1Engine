#include "core/input/CInput.h"

#include "../CDuskGUI.h"
#include "CDuskGUIFloatfield.h"
#include <cctype>

#include <sstream>
#include <iomanip>

#include "renderer/system/glDrawing.h"

//Handle CDuskGUI::CreateFloatfield	( const Handle& =0 )
// A floatfield w/ io
CDuskGUI::Handle CDuskGUI::CreateFloatfield ( const Handle & parent, const float defVal )
{
	std::stringstream ss;
	ss << std::setprecision(4) << defVal;

	vElements.push_back( new CDuskGUIFloatfield() );
	vElements.back()->parent = parent;
	((CDuskGUIFloatfield*)vElements.back())->label = ss.str();

	return Handle(vElements.size()-1);
}

// Update floatfield value
void CDuskGUI::UpdateFloatfield ( const Handle & handle, float & inOutFloatVal )
{
	CDuskGUIFloatfield* ff = (CDuskGUIFloatfield*)vElements[int(handle)];
	if ( fabs(ff->lastFloatValue-inOutFloatVal) > FTYPE_PRECISION )
	{
		ff->floatValue = inOutFloatVal;
		/*stringstream ss;
		ss << inOutFloatVal;
		//char temp [64];
		//sprintf( temp, "%f", inOutFloatVal );
		ff->label = ss.str();*/
		//ff->label = string( temp );
		/*char temp [64];
		sprintf( temp, "%f", inOutFloatVal );
		ff->label = temp;*/

		//if (( inOutFloatVal == inOutFloatVal )&&( inOutFloatVal <= FLT_MAX && inOutFloatVal >= -FLT_MAX ))
		if ( VALID_FLOAT(inOutFloatVal) )
		{
			std::stringstream ss;
			ss << std::setprecision(4) << inOutFloatVal;
			ff->label = ss.str();
				
			ff->floatValue = (float)(atof( ff->label.c_str() ));
			inOutFloatVal = ff->floatValue;
		}
		else
		{
			ff->label = "0";
			ff->floatValue = 0;
			inOutFloatVal = 0;
		}
	}
	else
	{
		inOutFloatVal = ff->floatValue;
	}
}

void CDuskGUIFloatfield::Update ( void )
{
	CDuskGUIElement::Update();

	lastFloatValue = floatValue;
	if ( hasFocus )
	{
		GetFloatValue();
	}

	// check for valid value
	int invalidCount = 0;
	for ( uint i = 0; i < label.length(); ++i ) {
		if ( !isalnum( label[i] ) ) {
			if ( label[i] == '.' || label[i] == '-' ) {
				invalidCount += 1;
			}
			else {
				invalidCount += 2;
			}
		}
		else if ( isalpha( label[i] ) && label[i] != 'e' ) {
			invalidCount += 1;
		}
	}
	if ( invalidCount >= 3 )
	{
		hasValidValue = false;
		floatValue = 0;
		if ( !hasFocus ) {
			label = "0";
		}
	}
	else if ( fabs( floatValue ) <= FTYPE_PRECISION )
	{
		hasValidValue = true;
		floatValue = 0;
		if ( !hasFocus ) {
			label = "0";
		}
	}
	else {
		hasValidValue = true;
	}
}
void CDuskGUIFloatfield::Render ( void )
{
	GLd_ACCESS

	// Material binding
	if ( mouseIn ) {
		setDrawHover();
	}
	else
	{
		if ( hasFocus )
			setDrawDown();
		else
			setDrawDefault();
	}

	if ( !hasValidValue ) {
		setSubdrawError();
	}
	else {
		setSubdrawDefault();
	}
	drawRectWire( rect );
	drawRect( rect );

	// Now draw text
	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
	//Real cursor_x = rect.pos.x + rect.size.x*0.01f + 0.012f + (GLd.GetAutoTextWidth( label.c_str() )/Screen::Info.width);
	//
	//if ( hasFocus )
	//{
	//	setSubdrawPulse();
	//	drawLine( cursor_x, rect.pos.y + rect.size.y*0.08f + 0.005f, cursor_x, rect.pos.y + rect.size.y*0.82f );
	//}
}

void CDuskGUIFloatfield::GetFloatValue ( void )
{
	if ( label.size() < 16 )
	{
		/*for ( int i = 0; i < 256; ++i )
		{
			if ( core::Input::keydown[i] )
			{
				if ( isalnum(i) || ispunct(i) )
				{
					label += char(i);
				}
				else if ( i == VK_OEM_PERIOD )
				{
					label += '.';
				}
				else if ( i == VK_OEM_MINUS )
				{
					label += '-';
				}
			}
		}*/
		if ( Input::KeydownAny() )
		{
			int i = Input::GetTypeChar();
			if ( isalnum(i) || ispunct(i) || (i=='.') || (i=='-') )
			{
				label += char(i);
			}
		}
	}
	if ( Input::Keydown( Keys.Backspace ) )//if ( core::Input::keydown[VK_BACK] )
	{
		//label = label.substr( label.size()-1 );
		if ( label.size() > 0 )
		{
			label.resize( label.size()-1 );
		}
	}

	floatValue = (float)(atof( label.c_str() ));
}