
// Include stuph
#include "../CDuskGUI.h"
#include "CDuskGUIColorpicker.h"
#include "../dialogues/DGUI_DialogueColorpicker.h"
#include "core/system/Screen.h" // Include screen properties
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/system/glMainSystem.h" // Include the main system
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

void CDuskGUIColorpicker::SetColor ( Color& c )
{
	Color hsl;
	c.GetHSLC( hsl );

	lastColorValue = c;
	colorValue = c;

	hue = hsl.red;
	saturation = hsl.green;
	lightness = hsl.blue; 
}

void CDuskGUIColorpicker::Update ( void )
{
	CDuskGUIButton::Update();

	if ( hue < 0 )
	{
		// compute hue
		Real chroma;

		// compute lightness
		Real fmax, fmin;
		fmax = std::max<Real>( std::max<Real>( colorValue.red, colorValue.green ), colorValue.blue );
		fmin = std::min<Real>( std::min<Real>( colorValue.red, colorValue.green ), colorValue.blue );
		lightness = ( fmax + fmin ) * 0.5f;
		chroma = fmax - fmin;

		// compute hue CORRECLTY.
		if ( chroma < 0.000001f )
			hue = 0;
		else if ( fmax == colorValue.red )
			hue = (colorValue.green-colorValue.blue)/chroma;
		else if ( fmax == colorValue.green )
			hue = (colorValue.blue-colorValue.red)/chroma + 2.0f;
		else if ( fmax = colorValue.blue )
			hue = (colorValue.red-colorValue.green)/chroma + 4.0f;
		else
			std::cout << "ERRRR" << std::endl;
		hue *= 60;

		// compute saturation
		if (( chroma > 0 )&&( lightness < 1 ))
			saturation = chroma/(1-fabs(2*lightness-1));
		else
			saturation = 0;
	}

	if ( (!visible) || (!drawn) )
		return;

	// Update color
	lastColorValue = colorValue;
	if ( !inDialogueMode )
	{
		// Button state
		homeRect = rect;
		if ( colorDialogue == -1 && isPressed )
		{
			// Create dialogue...
			colorDialogue = activeGUI->DialogueOpenColorpicker(colorValue, "Select Color");
			Dusk::DialogueColorpicker* colorpicker = (Dusk::DialogueColorpicker*)*colorDialogue;
			colorpicker->m_sourcePicker = activeGUI->hCurrentElement;
		}
		if ( colorDialogue != -1 && colorDialogue->m_type == 41 )
		{
			if (activeGUI->GetColorpickerValue( &colorValue, colorDialogue ))
			{	// Returns true when closing
				colorDialogue = -1;
			}
		}

		homeColorValue = colorValue;

		useMode = 0;
	}
	/*else
	{
		// Color picking state
		activeGUI->hCurrentDialogue = activeGUI->hCurrentElement;

		const int MODE_OFF		= 0;
		const int MODE_WHEEL	= 1;
		const int MODE_TRIANGLE	= 2;
		const int MODE_ALPHA	= 3;

		if ( mouseIn )
		{
			if ( CInput::Mouse(CInput::MBLeft) )
			{
				bool hasChange = false;
				Vector2d dpos, delta;
				Real dl, dist;
				Vector2d mouse_coords;
				
				// Get proper mouse coords
				mouse_coords = Vector2d( cursor_pos.x * Screen::Info.width, cursor_pos.y * Screen::Info.height );

				// Get center pos to work from
				dpos.x = (rect.pos.x+(rect.size.x*0.5f))*Screen::Info.width;
				dpos.y = (rect.pos.y+(rect.size.y*0.5f))*Screen::Info.height;
				dl = std::min<float>( (rect.size.x*0.4f*Screen::Info.width), (rect.size.y*0.4f*Screen::Info.height) );

				// Get delta from center and mouse
				delta = mouse_coords - dpos;
				dist = delta.magnitude(); // Calc distance

				// First check if mouse was in wheel
				if ( useMode == MODE_WHEEL ||(( dist < dl )&&( dist > dl*0.8f )))
				{
					if ( useMode == MODE_OFF ) {
						useMode = MODE_WHEEL;
					}
					if ( useMode == MODE_WHEEL ) {
						// Get the angle
						hue = (Real)radtodeg( atan2( delta.y, delta.x ) );
						if ( hue < 0 )
							hue += 360;

						hasChange = true;
					}
				}
				// Now check if mouse was in triangle
				else if ( useMode == MODE_TRIANGLE || (( delta.x > -dl*0.5f )&&( delta.x < dl*0.75f )&&( delta.y > -dl*0.6f )&&( delta.y < dl*0.6f )))
				{
					if ( useMode == MODE_OFF ) {
						useMode = MODE_TRIANGLE;
					}
					if ( useMode == MODE_TRIANGLE ) {
						// get the saturation value from x
						saturation = ( delta.x + dl*0.5f ) / ( dl*1.25f );
						// get lightness value from y
						lightness = 1.0f-(( delta.y + dl*0.6f*(1-saturation) ) / ( dl*1.2f*(1-saturation) ));
						// lerp to 0.5
						lightness += (0.5f-lightness) * saturation;

						hasChange = true;
					}
				}
				// Now check if mouse was in alpha bar
				else if ( useMode == MODE_ALPHA || (
						 ( cursor_pos.x > rect.pos.x+rect.size.x*0.82f )&&( cursor_pos.x < rect.pos.x+rect.size.x*0.88f )
					   &&( cursor_pos.y > rect.pos.y+rect.size.y*0.05f )&&( cursor_pos.y < rect.pos.y+rect.size.y*0.95f )))
				{
					if ( useMode == MODE_OFF ) {
						useMode = MODE_ALPHA;
					}
					if ( useMode == MODE_ALPHA ) {
						colorValue.alpha = 1.0f - (( cursor_pos.y - (rect.pos.y+rect.size.y*0.05f) ) / ( rect.size.y*0.9f ));
						if ( colorValue.alpha < 0 ) {
							colorValue.alpha = 0;
						}
						else if ( colorValue.alpha > 1 ) {
							colorValue.alpha = 1;
						}
					}
				}

				if ( hasChange )
				{
					// Update color with vals
					Real chroma = ( 1.0f - fabs( 2.0f*lightness - 1.0f )) * saturation;
					Real hue_t = hue / 60.0f;
					Real cx_t = chroma * ( 1 - fabs( ( (hue_t*0.5f - int(hue_t*0.5f))*2.0f ) - 1.0f ) );
					Color color_t;
					if ( hue_t < 1 )
						color_t = Color( chroma, cx_t, 0 );
					else if ( hue_t < 2 )
						color_t = Color( cx_t, chroma, 0 );
					else if ( hue_t < 3 )
						color_t = Color( 0, chroma, cx_t );
					else if ( hue_t < 4 )
						color_t = Color( 0, cx_t, chroma );
					else if ( hue_t < 5 )
						color_t = Color( cx_t, 0, chroma );
					else if ( hue_t < 6 )
						color_t = Color( chroma, 0, cx_t );
					else
						color_t = Color( 0.0f,0,0 );
					Real cm_t = lightness - chroma*0.5f;
					colorValue.red	= color_t.red	+ cm_t;
					colorValue.green= color_t.green + cm_t;
					colorValue.blue = color_t.blue	+ cm_t;
				}
			}
			else
			{
				useMode = 0;
			}
		}
		else
		{
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
				lastColorValue = homeColorValue;
			}
			else if ( CInput::MouseDown(CInput::MBRight) )
			{
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
				lastColorValue = homeColorValue;
			}
		}
	}*/
}

void CDuskGUIColorpicker::Render ( void )
{
	GL_ACCESS GLd_ACCESS

	Rect temp = rect;
	rect = homeRect;

		// Draw the background
		{
			if ( mouseIn )
			{
				if ( beginPress )
					setDrawDown();
				else
					setDrawHover();
			}
			else
			{
				setDrawDefault();
			}

			// Draw background
			drawRect( rect );
			// Draw background outline
			drawRectWire( rect );
		}

		// Draw the rect showing current color
		setDrawDefault();
		setSubdrawOverrideColor( Color(colorValue.red, colorValue.green, colorValue.blue, 1.0) );
		drawRect( rect );
		// Draw a rectangle for the current alpha value
		setSubdrawOverrideColor( Color(0.0, 0.0, 0.0, 1.0) );
		drawRect( Rect(rect.pos.x, rect.pos.y + rect.size.y * 0.9F, rect.size.x, rect.size.y * 0.1F) );
		setSubdrawOverrideColor( Color(1.0, 1.0, 1.0, 1.0) );
		drawRect( Rect(rect.pos.x, rect.pos.y + rect.size.y * 0.9F, rect.size.x * colorValue.alpha, rect.size.y * 0.1F) );

		// Draw the border
		{
			if ( mouseIn )
			{
				if ( beginPress )
					setDrawDown();
				else
					setDrawHover();
			}
			else
			{
				setDrawDefault();
			}
			setSubdrawDefault();

			// Draw outline
			drawRectWire( rect );
		}

	rect = temp;

	if ( inDialogueMode )
	{
		//activeGUI->matDefault->bind();
		setDrawDefault();
		setSubdrawDefault();

		/*glColor4f(
			activeGUI->matDefault->diffuse.red * 0.6f,
			activeGUI->matDefault->diffuse.green * 0.6f,
			activeGUI->matDefault->diffuse.blue * 0.6f,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_WIRE );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );

		glColor4f(
			activeGUI->matDefault->diffuse.red,
			activeGUI->matDefault->diffuse.green,
			activeGUI->matDefault->diffuse.blue,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_FLAT );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );*/
		drawRectWire( rect );
		drawRect( rect );

		// draw the current color
		//glColor4f( colorValue.red,colorValue.green,colorValue.blue,colorValue.alpha );
		//GLd.DrawSet2DMode( GL.D2D_FLAT );
		//GLd.DrawRectangleA( rect.pos.x+0.02f, rect.pos.y+0.07f, rect.size.x*0.12f, rect.size.y*0.09f );
		setSubdrawOverrideColor( colorValue );
		drawRect( Rect( rect.pos.x+0.02f, rect.pos.y+0.07f, rect.size.x*0.12f, rect.size.y*0.09f ) );


		//GL.prepareDraw();

		//GL.beginOrtho( 0,0, 1,1, -45,45 );
		core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
		//GL.beginOrtho();
		GLd.DrawSet2DScaleMode();

		activeGUI->matDefault->bindPass(0);
		//activeGUI->matDefault->setShaderConstants( activeGUI );

		{
			// Now, draw the color wheel
			Vector2d dpos;
			float dl;
			dpos.x = (rect.pos.x+(rect.size.x*0.5f))*Screen::Info.width;
			dpos.y = (rect.pos.y+(rect.size.y*0.5f))*Screen::Info.height;
			dl = std::min<Real>( (rect.size.x*0.4f*Screen::Info.width), (rect.size.y*0.4f*Screen::Info.height) );
			Vector2d dopos1, dopos2;
			Color dcolor1, dcolor2;
			int modColor;
			auto lPrimWheel = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
			for ( int i = 0; i <= 360; i += 5 )
			{
				dopos1.x = dpos.x + (Real)( cos( degtorad( i ) ) * dl );
				dopos1.y = dpos.y + (Real)( sin( degtorad( i ) ) * dl );

				dopos2.x = dpos.x + (Real)( cos( degtorad( i ) ) * dl * 0.8f );
				dopos2.y = dpos.y + (Real)( sin( degtorad( i ) ) * dl * 0.8f );

				modColor = i % 60;
				if (( i < 60 )||( i == 360 ))
					dcolor1 = Color( 1.0f, modColor/60.0f, 0.0f );
				else if ( i < 120 )
					dcolor1 = Color( 1.0f - modColor/60.0f, 1.0f, 0.0f );
				else if ( i < 180 )
					dcolor1 = Color( 0.0f, 1.0f, modColor/60.0f );
				else if ( i < 240 )
					dcolor1 = Color( 0.0f, 1.0f - modColor/60.0f, 1.0f );
				else if ( i < 300 )
					dcolor1 = Color( modColor/60.0f, 0.0f, 1.0f );
				else
					dcolor1 = Color( 1.0f, 0.0f, 1.0f - modColor/60.0f );
				dcolor1.alpha = 1.0f;
				
				GLd.P_PushColor( dcolor1 );
				GLd.P_AddVertex( dopos1.x,dopos1.y );
				GLd.P_AddVertex( dopos2.x,dopos2.y );
			}
			GLd.EndPrimitive(lPrimWheel);

			// Draw the position of the current hue
			auto lPrimPos = GLd.BeginPrimitive( GL_LINE_STRIP, activeGUI->matDefault );
				GLd.P_PushColor( 0.2f,0.2f,0.2f,0.8f );

				dopos1.x = dpos.x + (Real)( cos( degtorad( hue-2 ) ) * dl * 1.03f );
				dopos1.y = dpos.y + (Real)( sin( degtorad( hue-2 ) ) * dl * 1.03f );

				dopos2.x = dpos.x + (Real)( cos( degtorad( hue-2 ) ) * dl * 0.77f );
				dopos2.y = dpos.y + (Real)( sin( degtorad( hue-2 ) ) * dl * 0.77f );

				GLd.P_AddVertex( dopos1.x,dopos1.y );
				GLd.P_AddVertex( dopos2.x,dopos2.y );

				dopos1.x = dpos.x + (Real)( cos( degtorad( hue+2 ) ) * dl * 1.03f );
				dopos1.y = dpos.y + (Real)( sin( degtorad( hue+2 ) ) * dl * 1.03f );

				dopos2.x = dpos.x + (Real)( cos( degtorad( hue+2 ) ) * dl * 0.77f );
				dopos2.y = dpos.y + (Real)( sin( degtorad( hue+2 ) ) * dl * 0.77f );

				GLd.P_AddVertex( dopos2.x,dopos2.y );
				GLd.P_AddVertex( dopos1.x,dopos1.y );

				dopos1.x = dpos.x + (Real)( cos( degtorad( hue-2 ) ) * dl * 1.03f );
				dopos1.y = dpos.y + (Real)( sin( degtorad( hue-2 ) ) * dl * 1.03f );

				GLd.P_AddVertex( dopos1.x,dopos1.y );

			GLd.EndPrimitive(lPrimPos);

			// Draw the saturation/lightness triangle
			auto lPrimTri = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );

			{
				int i = int(hue);
				modColor = i % 60;
				if (( i < 60 )||( i == 360 ))
					dcolor1 = Color( 1.0f, modColor/60.0f, 0.0f );
				else if ( i < 120 )
					dcolor1 = Color( 1.0f - modColor/60.0f, 1.0f, 0.0f );
				else if ( i < 180 )
					dcolor1 = Color( 0.0f, 1.0f, modColor/60.0f );
				else if ( i < 240 )
					dcolor1 = Color( 0.0f, 1.0f - modColor/60.0f, 1.0f );
				else if ( i < 300 )
					dcolor1 = Color( modColor/60.0f, 0.0f, 1.0f );
				else
					dcolor1 = Color( 1.0f, 0.0f, 1.0f - modColor/60.0f );
				dcolor1.alpha = 1.0f;
			}
				//glColor4f( colorValue.red,colorValue.green,colorValue.blue,1.0f );
				GLd.P_PushColor( dcolor1 );
				GLd.P_AddVertex( dpos.x + dl*0.75f, dpos.y );
				GLd.P_PushColor( 1.0f,1.0f,1.0f,1.0f );
				GLd.P_AddVertex( dpos.x - dl*0.5f, dpos.y - dl*0.6f );
				GLd.P_PushColor( 0.0f,0.0f,0.0f,1.0f );
				GLd.P_AddVertex( dpos.x - dl*0.5f, dpos.y + dl*0.6f );
			GLd.EndPrimitive(lPrimTri);

			// Draw the saturation/lightness selection
			dopos1.x = dpos.x - dl*0.5f + dl*(0.75f+0.5f)*saturation;
			//dopos1.y = dpos.y + dl*( 0.6f - 1.2f*lightness )*(1-saturation);
			dopos1.y = dpos.y + dl*( 0.6f - 1.2f*lightness );
			if ( lightness > 0.35f )
				GLd.P_PushColor( 0.0f,0.0f,0.0f,1.0f );
			else
				GLd.P_PushColor( 1.0f,1.0f,1.0f,1.0f );
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			GLd.DrawRectangleA( dopos1.x/Screen::Info.width-0.002f,dopos1.y/Screen::Info.height-0.002f,0.004f,0.004f );
			//GL.DrawCircleA( dopos1.x,dopos1.y, 0.03f );

			// Draw the alpha bar on the right
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			if ( lightness > 0.35f )
				GLd.P_PushColor( 0.0f,0.0f,0.0f,0.0f );
			else
				GLd.P_PushColor( 1.0f,1.0f,1.0f,0.0f );
			GLd.DrawRectangleA( rect.pos.x+rect.size.x*0.82f,rect.pos.y+rect.size.y*0.05f,rect.size.x*0.06f,rect.size.y*0.9f );
			auto lPrimAlpha = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
				GLd.P_PushColor( colorValue.red, colorValue.green, colorValue.blue, 1.0f );
				GLd.P_AddVertex(
					(rect.pos.x+rect.size.x*0.82f+rect.size.x*0.06f)*Screen::Info.width,
					(rect.pos.y+rect.size.y*0.05f)*Screen::Info.height );
				GLd.P_AddVertex(
					(rect.pos.x+rect.size.x*0.82f)*Screen::Info.width,
					(rect.pos.y+rect.size.y*0.05f)*Screen::Info.height );
				GLd.P_PushColor( colorValue.red, colorValue.green, colorValue.blue, 0.0f );
				GLd.P_AddVertex(
					(rect.pos.x+rect.size.x*0.82f+rect.size.x*0.06f)*Screen::Info.width,
					(rect.pos.y+rect.size.y*0.05f+rect.size.y*0.9f)*Screen::Info.height );
				GLd.P_AddVertex(
					(rect.pos.x+rect.size.x*0.82f)*Screen::Info.width,
					(rect.pos.y+rect.size.y*0.05f+rect.size.y*0.9f)*Screen::Info.height );
			GLd.EndPrimitive(lPrimAlpha);
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			//glColor4f( 0.0f,0.0f,0.0f,1.0f );
			GLd.P_PushColor( 0.0f,0.0f,0.0f,1.0f );
			GLd.DrawRectangleA( rect.pos.x+rect.size.x*0.82f,rect.pos.y+rect.size.y*0.05f,rect.size.x*0.06f,rect.size.y*0.9f );

			// Draw alpha bar selector
			GLd.DrawRectangleA(
				rect.pos.x+rect.size.x*0.82f-0.005f,
				rect.pos.y+rect.size.y*0.05f+(rect.size.y*0.9f*(1-colorValue.alpha))-0.005f,
				rect.size.x*0.06f+0.010f,
				0.010f );

			//GL.endOrtho();
			//GL.cleanupDraw();
		}

		//activeGUI->matDefault->unbind();

		// Now draw text
		activeGUI->matFont->bindPass(0);
		activeGUI->fntDefault->Set();
			
			GLd.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );

			GLd.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.015f, rect.pos.y + rect.size.y*0.48f + 0.03f, "R: %.3lf", colorValue.red );
			GLd.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.015f, rect.pos.y + rect.size.y*0.54f + 0.03f, "G: %.3lf", colorValue.green );
			GLd.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.015f, rect.pos.y + rect.size.y*0.60f + 0.03f, "B: %.3lf", colorValue.blue );
			GLd.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.015f, rect.pos.y + rect.size.y*0.66f + 0.03f, "A: %.3lf", colorValue.alpha );

		//activeGUI->fntDefault->Unbind();
		//activeGUI->matFont->unbind();
	}
}