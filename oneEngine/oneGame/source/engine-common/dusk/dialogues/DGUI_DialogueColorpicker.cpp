#include "core/input/CInput.h"

#include "DGUI_DialogueColorpicker.h"
#include "../CDuskGUI.h"
#include "../controls/CDuskGUIColorpicker.h"

#include "core/math/Color.h"
#include "core/math/Math.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/object/immediate/immediate.h"

#include <algorithm>

Dusk::Handle CDuskGUI::DialogueOpenColorpicker ( const Color& n_initialColor, const char* nDialogueTitle )
{
	if ( hCurrentDialogue == -1 )
	{
		vElements.push_back( new Dusk::DialogueColorpicker() );

		Handle handle ( vElements.size()-1 );
		Dusk::DialogueColorpicker* colorpicker = (Dusk::DialogueColorpicker*)*handle;

		colorpicker->m_currentColor = n_initialColor;
		colorpicker->inDialogueMode = true;
		colorpicker->label = nDialogueTitle;
		colorpicker->rect = Rect( 100,100, 300,500 );

		// Set the current dialogue
		hCurrentDialogue = handle;
		// Save the handle
		return handle;
	}
	return -1;
}

bool CDuskGUI::GetColorpickerValue ( Color* n_outColor, const Handle& handleOverride )
{
	if ( hCurrentDialogue >= 0 && hCurrentDialogue->m_type == 41 )
	{
		if ( ((int)handleOverride != -1) && hCurrentDialogue != handleOverride ) {
			return false;
		}
		Dusk::DialogueColorpicker* colorpicker = (Dusk::DialogueColorpicker*)*hCurrentDialogue;
		*n_outColor = colorpicker->m_currentColor;
		if ( colorpicker->hasSelection )
		{
			DeleteElement( hCurrentDialogue );
			hCurrentDialogue = -1;
			return true;
		}
	}
	return false;
}

bool CDuskGUI::ColorpickerDialogueHasSelection ( const Handle& handleOverride )
{
	if ( hCurrentDialogue >= 0 && hCurrentDialogue->m_type == 41 )
	{
		if ( hCurrentDialogue != handleOverride ) {
			return false;
		}
		Dusk::DialogueColorpicker* colorpicker = (Dusk::DialogueColorpicker*)*hCurrentDialogue;
		if ( colorpicker->endMe )
		{
			DeleteElement( hCurrentDialogue );
			hCurrentDialogue = -1;
			return false;
		}
		return colorpicker->hasSelection;
	}
	else if ( hCurrentDialogue == -1 )
	{
		Handle handle ( handleOverride );
		Dusk::DialogueColorpicker* colorpicker = (Dusk::DialogueColorpicker*)*hCurrentDialogue;
		if ( colorpicker && colorpicker->endMe )
		{
			DeleteElement( handle );
		}
	}
	return false;
}


// == Dialogue Code ==
Dusk::DialogueColorpicker::DialogueColorpicker ( const int moverride )
	: CDuskGUIDialogue(moverride), hasSelection(false), endMe(false),
	m_sourcePicker(-1)
{
	// Create the child elements here
}

void Dusk::DialogueColorpicker::Update ( void )
{
	// Update drag logic
	if ( dragging )
	{
		drag_now = Vector2d( Input::MouseX(), Input::MouseY() );
	}

	// Update hover mode or drag logic
	current_mode_hover = Mode::None;
	updateColorSliders( Vector2d(rect.pos.x + 30, rect.pos.y + 300) );
	if ( current_mode_hover == Mode::None && !rect.Contains(Vector2d( Input::MouseX(), Input::MouseY() )) )
	{
		current_mode_hover = Mode::Outside;
	}

	// Process mouse clicks
	if ( Input::MouseDown(Input::MBLeft) )
	{
		// Drag on mouse press
		if ( current_mode_hover != Mode::None && current_mode_hover != Mode::Outside )
		{
			// Set start points
			color_start = m_currentColor;
			drag_start = Vector2d( Input::MouseX(), Input::MouseY() );
			drag_now = drag_start;
			// Start dragging
			current_mode = current_mode_hover;
			dragging = true;
		}
		// Click outside to finish
		if ( current_mode_hover == Mode::Outside )
		{
			hasSelection = true;
		}
	}
	else if ( Input::MouseUp(Input::MBLeft) )
	{
		// Release drag on mouse up
		current_mode = Mode::None;
		dragging = false;
	}

	// Update picker (if any)
	if (m_sourcePicker != -1)
	{
		CDuskGUIColorpicker* picker = dynamic_cast<CDuskGUIColorpicker*>( *m_sourcePicker );
		if ( picker != NULL )
		{
			picker->Update();
		}
		else
		{
			delete_me = true;
			if ( activeGUI->hCurrentDialogue == activeGUI->hCurrentElement )
				activeGUI->hCurrentDialogue = -1;
		}
	}
}

void Dusk::DialogueColorpicker::Render ( void )
{
	GL_ACCESS GLd_ACCESS;

	bool pixelMode = activeGUI->bInPixelMode;
	activeGUI->bInPixelMode = true;

	// Render the main panel
	setDrawDefault();
	setSubdrawDefault();
	drawRect( rect );
	drawRectWire( rect );

	// Draw the prompt text
	drawText( rect.pos.x + 8, rect.pos.y + 24, label.c_str() );

	// Draw the color wheel
	drawColorWheel( Vector2d(rect.pos.x + 150, rect.pos.y + 150) );
	// Draw color sliders
	drawColorSliders( Vector2d(rect.pos.x + 30, rect.pos.y + 300) );

	activeGUI->bInPixelMode = pixelMode;
}


void Dusk::DialogueColorpicker::drawColorWheel ( const Vector2d& position )
{
	GL_ACCESS GLd_ACCESS;

	//GL.prepareDraw();
	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//GL.beginOrtho();
	//GLd.DrawSet2DScaleMode();

	//activeGUI->matDefault->bindPass(0);
	//activeGUI->matDefault->setShaderConstants( activeGUI );

	// Draw the color circle
	{
		const float circle_radius_inner = 50.0F;
		const float circle_radius_outer = 120.0F;

		Vector2d popos1, popos2;
		Vector2d dopos1, dopos2;
		Color pcolor1, pcolor2;
		Color dcolor1, dcolor2;
		int modColor;

		//auto lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		for ( int i = 0; i <= 360; i += 1 )
		{
			// Save previous state
			popos1 = dopos1;
			popos2 = dopos2;
			pcolor1 = dcolor1;
			pcolor2 = dcolor2;

			// Select positions
			dopos1.x = position.x + (Real)( cos( degtorad( i ) ) * circle_radius_outer );
			dopos1.y = position.y + (Real)( sin( degtorad( i ) ) * circle_radius_outer );

			dopos2.x = position.x + (Real)( cos( degtorad( i ) ) * circle_radius_inner );
			dopos2.y = position.y + (Real)( sin( degtorad( i ) ) * circle_radius_inner );

			// Select color
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

			Color tempHSL;
			dcolor1.GetHSLC(tempHSL);
			tempHSL[1] = 0.0F;
			dcolor2.SetHSL(tempHSL);

			//GLd.P_PushColor( dcolor1 );
			//GLd.P_AddVertex( dopos1.x,dopos1.y );
			//GLd.P_PushColor( dcolor2 );
			//GLd.P_AddVertex( dopos2.x,dopos2.y );

			if (i == 0) continue;

			arModelVertex vertices [4];
			memset(vertices, 0, sizeof(arModelVertex) * 4);

			vertices[0].position = popos1;
			vertices[1].position = popos2;
			vertices[2].position = dopos1;
			vertices[3].position = dopos2;

			vertices[0].color = Vector4f(pcolor1.raw);
			vertices[1].color = Vector4f(pcolor2.raw);
			vertices[2].color = Vector4f(dcolor1.raw);
			vertices[3].color = Vector4f(dcolor2.raw);

			getMeshBuilder()->addQuad( vertices, false );
		}
		//GLd.EndPrimitive(lPrim);
	}

	// Draw the circle of the current color
	{
		const float circle_radius_outer = 40.0F;
		const float circle_radius_inner = 20.0F;

		Vector2d popos1, popos2;
		Vector2d dopos1, dopos2;

		for ( int i = 0; i <= 360; i += 5 )
		{
			// Save previous state
			popos1 = dopos1;
			popos2 = dopos2;

			// Set positions
			dopos1.x = position.x + (Real)( cos( degtorad( i ) ) * circle_radius_outer );
			dopos1.y = position.y + (Real)( sin( degtorad( i ) ) * circle_radius_outer );

			dopos2.x = position.x + (Real)( cos( degtorad( i ) ) * circle_radius_inner );
			dopos2.y = position.y + (Real)( sin( degtorad( i ) ) * circle_radius_inner );

			//GLd.P_AddVertex( dopos1.x,dopos1.y );
			//GLd.P_AddVertex( dopos2.x,dopos2.y );

			if (i == 0) continue;

			// Add mesh
			arModelVertex vertices [4];
			memset(vertices, 0, sizeof(arModelVertex) * 4);

			vertices[0].position = popos1;
			vertices[1].position = popos2;
			vertices[2].position = dopos1;
			vertices[3].position = dopos2;

			vertices[0].color = Vector4f(m_currentColor.raw);
			vertices[1].color = Vector4f(m_currentColor.raw);
			vertices[2].color = Vector4f(m_currentColor.raw);
			vertices[3].color = Vector4f(m_currentColor.raw);

			getMeshBuilder()->addQuad( vertices, false );
		}
	}


	//GL.endOrtho();
	//GL.cleanupDraw();
}

void Dusk::DialogueColorpicker::updateColorSliders ( const Vector2d& position )
{
	// Update hover based on mouse position
	if (!dragging)
	{
		Vector2d mousepos = Vector2d( Input::MouseX(), Input::MouseY() );

		// Select the RGB mouse over
		{
			float y = 0;
			const float width = 240;
			const float height = 15;

			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::R;
			y += 25;
			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::G;
			y += 25;
			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::B;
		}
		// Select the HSV mouseover
		{
			float y = 100;
			const float width = 240;
			const float height = 15;

			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::H;
			y += 25;
			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::S;
			y += 25;
			if ( Rect( position.x, position.y + y, width, height ).Contains(mousepos) )
				current_mode_hover = Mode::V;
		}
	}
	else
	{
		Color hsl;
		color_start.GetHSLC(hsl);

		switch (current_mode)
		{
		case Mode::R:
			m_currentColor[0] = math::clamp( color_start[0] + (drag_now.x - drag_start.x) / 255.0F, 0.0F, 1.0F );
			break;
		case Mode::G:
			m_currentColor[1] = math::clamp( color_start[1] + (drag_now.x - drag_start.x) / 255.0F, 0.0F, 1.0F );
			break;
		case Mode::B:
			m_currentColor[2] = math::clamp( color_start[2] + (drag_now.x - drag_start.x) / 255.0F, 0.0F, 1.0F );
			break;
		case Mode::H:
			hsl[0] = math::clamp( hsl[0] + (drag_now.x - drag_start.x) / 360.0F, 0.0F, 360.0F );
			m_currentColor.SetHSL(hsl);
			break;
		case Mode::S:
			hsl[1] = math::clamp( hsl[1] + (drag_now.x - drag_start.x) / 255.0F, 0.0F, 1.0F );
			m_currentColor.SetHSL(hsl);
			break;
		case Mode::V:
			hsl[2] = math::clamp( hsl[2] + (drag_now.x - drag_start.x) / 255.0F, 0.0F, 1.0F );
			m_currentColor.SetHSL(hsl);
			break;
		}
	}
}
void Dusk::DialogueColorpicker::drawColorSliders ( const Vector2d& position )
{
	GL_ACCESS GLd_ACCESS;

	//GL.prepareDraw();
	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//GL.beginOrtho();
	//GLd.DrawSet2DScaleMode();

	//activeGUI->matDefault->bindPass(0);
	//activeGUI->matDefault->setShaderConstants( activeGUI );

	// Draw the RGB bars
	{
		float y = 0;
		float x_sub = 0;

		const float width = 240;
		const float height = 15;
		const float selection_hwidth = 3;
		const float selection_hheight = 3;

		arModelVertex vertices [4];
		memset(vertices, 0, sizeof(arModelVertex) * 4);

		//auto lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		//GLd.P_PushColor( 0.0F, m_currentColor[1], m_currentColor[2] );
		//GLd.P_AddVertex( position.x, position.y + y );
		//GLd.P_AddVertex( position.x, position.y + y + height );
		//GLd.P_PushColor( 1.0F, m_currentColor[1], m_currentColor[2] );
		//GLd.P_AddVertex( position.x + width, position.y + y );
		//GLd.P_AddVertex( position.x + width, position.y + y + height );
		//GLd.EndPrimitive(lPrim);
		vertices[0].position = position + Vector2f(0, y);
		vertices[1].position = position + Vector2f(0, y + height);
		vertices[2].position = position + Vector2f(width, y);
		vertices[3].position = position + Vector2f(width, y + height);
		vertices[0].color = Vector4f(0.0F, m_currentColor[1], m_currentColor[2], 1.0F);
		vertices[1].color = Vector4f(0.0F, m_currentColor[1], m_currentColor[2], 1.0F);
		vertices[2].color = Vector4f(1.0F, m_currentColor[1], m_currentColor[2], 1.0F);
		vertices[3].color = Vector4f(1.0F, m_currentColor[1], m_currentColor[2], 1.0F);
		getMeshBuilder()->addQuad(vertices, false);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * m_currentColor[0];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				 Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);

		y += 25;

		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		//GLd.P_PushColor( m_currentColor[0], 0.0F, m_currentColor[2] );
		//GLd.P_AddVertex( position.x, position.y + y );
		//GLd.P_AddVertex( position.x, position.y + y + height );
		//GLd.P_PushColor( m_currentColor[0], 1.0F, m_currentColor[2] );
		//GLd.P_AddVertex( position.x + width, position.y + y );
		//GLd.P_AddVertex( position.x + width, position.y + y + height );
		//GLd.EndPrimitive(lPrim);
		vertices[0].position = position + Vector2f(0, y);
		vertices[1].position = position + Vector2f(0, y + height);
		vertices[2].position = position + Vector2f(width, y);
		vertices[3].position = position + Vector2f(width, y + height);
		vertices[0].color = Vector4f(m_currentColor[0], 0.0F, m_currentColor[2], 1.0F);
		vertices[1].color = Vector4f(m_currentColor[0], 0.0F, m_currentColor[2], 1.0F);
		vertices[2].color = Vector4f(m_currentColor[0], 1.0F, m_currentColor[2], 1.0F);
		vertices[3].color = Vector4f(m_currentColor[0], 1.0F, m_currentColor[2], 1.0F);
		getMeshBuilder()->addQuad(vertices, false);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * m_currentColor[1];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);

		y += 25;

		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		//GLd.P_PushColor( m_currentColor[0], m_currentColor[1], 0.0F );
		//GLd.P_AddVertex( position.x, position.y + y );
		//GLd.P_AddVertex( position.x, position.y + y + height );
		//GLd.P_PushColor( m_currentColor[0], m_currentColor[1], 1.0F );
		//GLd.P_AddVertex( position.x + width, position.y + y );
		//GLd.P_AddVertex( position.x + width, position.y + y + height );
		//GLd.EndPrimitive(lPrim);
		vertices[0].position = position + Vector2f(0, y);
		vertices[1].position = position + Vector2f(0, y + height);
		vertices[2].position = position + Vector2f(width, y);
		vertices[3].position = position + Vector2f(width, y + height);
		vertices[0].color = Vector4f(m_currentColor[0], m_currentColor[1], 0.0F, 1.0F);
		vertices[1].color = Vector4f(m_currentColor[0], m_currentColor[1], 0.0F, 1.0F);
		vertices[2].color = Vector4f(m_currentColor[0], m_currentColor[1], 1.0F, 1.0F);
		vertices[3].color = Vector4f(m_currentColor[0], m_currentColor[1], 1.0F, 1.0F);
		getMeshBuilder()->addQuad(vertices, false);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * m_currentColor[2];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);
	}

	// Draw the HSL bars
	{
		float y = 100;
		float x_sub = 0;

		const float width = 240;
		const float height = 15;
		const float selection_hwidth = 3;
		const float selection_hheight = 3;
		const int kDiv = 2;

		arModelVertex vertices [4];
		memset(vertices, 0, sizeof(arModelVertex) * 4);

		Color colorhsl_ref;
		m_currentColor.GetHSLC(colorhsl_ref);

		Color color0;
		Color color1;

		//auto lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		for (int i = 0; i <= (int)width; i += kDiv)
		{
			color0 = color1;
			color1.SetHSL( Color(i / width, colorhsl_ref[1], colorhsl_ref[2]) );
			//GLd.P_PushColor( color0 );
			//GLd.P_AddVertex( position.x + i, position.y + y );
			//GLd.P_AddVertex( position.x + i, position.y + y + height );

			if (i == 0) continue;
			vertices[0].position = position + Vector2f((Real)i, y);
			vertices[1].position = position + Vector2f((Real)i, y + height);
			vertices[2].position = position + Vector2f((Real)i + kDiv, y);
			vertices[3].position = position + Vector2f((Real)i + kDiv, y + height);
			vertices[0].color = Vector4f(color0.raw);
			vertices[1].color = Vector4f(color0.raw);
			vertices[2].color = Vector4f(color1.raw);
			vertices[3].color = Vector4f(color1.raw);
			getMeshBuilder()->addQuad(vertices, false);
		}
		//GLd.EndPrimitive(lPrim);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * colorhsl_ref[0];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);

		y += 25;

		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		for (int i = 0; i <= (int)width; i += kDiv)
		{
			color0 = color1;
			color1.SetHSL( Color(colorhsl_ref[0], i / width, colorhsl_ref[2]) );
			//GLd.P_PushColor( color0 );
			//GLd.P_AddVertex( position.x + i, position.y + y );
			//GLd.P_AddVertex( position.x + i, position.y + y + height );

			if (i == 0) continue;
			vertices[0].position = position + Vector2f((Real)i, y);
			vertices[1].position = position + Vector2f((Real)i, y + height);
			vertices[2].position = position + Vector2f((Real)i + kDiv, y);
			vertices[3].position = position + Vector2f((Real)i + kDiv, y + height);
			vertices[0].color = Vector4f(color0.raw);
			vertices[1].color = Vector4f(color0.raw);
			vertices[2].color = Vector4f(color1.raw);
			vertices[3].color = Vector4f(color1.raw);
			getMeshBuilder()->addQuad(vertices, false);
		}
		//GLd.EndPrimitive(lPrim);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * colorhsl_ref[1];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);

		y += 25;

		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		for (int i = 0; i <= (int)width; i += kDiv)
		{
			color0 = color1;
			color1.SetHSL( Color(colorhsl_ref[0], colorhsl_ref[1], i / width) );
			//GLd.P_PushColor( color0 );
			//GLd.P_AddVertex( position.x + i, position.y + y );
			//GLd.P_AddVertex( position.x + i, position.y + y + height );

			if (i == 0) continue;
			vertices[0].position = position + Vector2f((Real)i, y);
			vertices[1].position = position + Vector2f((Real)i, y + height);
			vertices[2].position = position + Vector2f((Real)i + kDiv, y);
			vertices[3].position = position + Vector2f((Real)i + kDiv, y + height);
			vertices[0].color = Vector4f(color0.raw);
			vertices[1].color = Vector4f(color0.raw);
			vertices[2].color = Vector4f(color1.raw);
			vertices[3].color = Vector4f(color1.raw);
			getMeshBuilder()->addQuad(vertices, false);
		}
		//GLd.EndPrimitive(lPrim);

		// Draw a box over current color
		//lPrim = GLd.BeginPrimitive( GL_TRIANGLE_STRIP, activeGUI->matDefault );
		x_sub = width * colorhsl_ref[2];
		//GLd.P_PushColor( m_currentColor );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub - selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y - selection_hheight );
		//GLd.P_AddVertex( position.x + x_sub + selection_hwidth, position.y + y + height + selection_hheight );
		//GLd.EndPrimitive(lPrim);
		getMeshBuilder()->addRect(
			Rect(position + Vector2f(x_sub - selection_hwidth, y - selection_hheight),
				Vector2f(selection_hwidth * 2.0F, height + selection_hheight * 2.0F)),
			m_currentColor,
			false
		);
	}

	//GL.endOrtho();
	//GL.cleanupDraw();

	// Draw RGB text
	drawText( position.x - 12, position.y +  0 + 13, "R" );
	drawText( position.x - 12, position.y + 25 + 13, "G" );
	drawText( position.x - 12, position.y + 50 + 13, "B" );

	drawText( position.x + 240 + 4, position.y +  0 + 13, std::to_string((int)(m_currentColor[0] * 255)).c_str() );
	drawText( position.x + 240 + 4, position.y + 25 + 13, std::to_string((int)(m_currentColor[1] * 255)).c_str() );
	drawText( position.x + 240 + 4, position.y + 50 + 13, std::to_string((int)(m_currentColor[2] * 255)).c_str() );

	// Draw HSV text
	drawText( position.x - 12, position.y + 100 + 13, "H" );
	drawText( position.x - 12, position.y + 125 + 13, "S" );
	drawText( position.x - 12, position.y + 150 + 13, "V" );

	Color colorhsl_ref;
	m_currentColor.GetHSLC(colorhsl_ref);
	drawText( position.x + 240 + 4, position.y + 100 + 13, std::to_string((int)(colorhsl_ref[0] * 360)).c_str() );
	drawText( position.x + 240 + 4, position.y + 125 + 13, std::to_string((int)(colorhsl_ref[1] * 255)).c_str() );
	drawText( position.x + 240 + 4, position.y + 150 + 13, std::to_string((int)(colorhsl_ref[2] * 255)).c_str() );
}