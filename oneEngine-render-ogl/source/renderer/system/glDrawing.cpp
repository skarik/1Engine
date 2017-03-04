#include "glDrawing.h"

#include "core/system/Screen.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"
#include "renderer/state/Settings.h"

// The Global GL drawing object
//glDrawing GLd;
glDrawing* glDrawing::ActiveSystem = NULL;

// == Constructor and Initialization ==
glDrawing::glDrawing ( void )
{
	ActiveSystem = this;

	// States
	iPrim2DDrawMode = D2D_WIRE;
	iPrim2DScalMode = SCALE_DEFAULT;
	iPrim2DRoundMode= RND_DONT_ROUND;

	// Primitive info
	prim_vertex_list	= NULL;
	// Initialize drawing
	MarkBuffersFreeUsage();
}
glDrawing::~glDrawing ( void )
{
	if ( ActiveSystem == this )
	{
		ActiveSystem = NULL;
	}
}
// System's current active reference
glDrawing* glDrawing::ActiveReference ( void )
{
	return ActiveSystem;
}

// Cleanup after old frame
void glDrawing::MarkBuffersFreeUsage ( void )
{
	// Go through the primitive list and free up all their usage
	for ( auto stream = prim_list.begin(); stream != prim_list.end(); ++stream )
	{
		stream->inUse = false;
	}
	prim_count = 0;

	//std::swap( prim_list, prim_list_swap );
	//prim_list.swap( prim_list_swap );
}

//===============================================================================================//
// Rendering
//===============================================================================================//
void	glDrawing::DrawText ( float dx, float dy, const char* fmt, ... )
{
	GL_ACCESS; // Using the glMainSystem accessor
	/*
	return;
	va_list		ap;	// Argument list
	static char	text[256];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	GL.prepareDraw();

	// Change offset behavior based on the font type
	if ( !fontInfo.isTexture )
	{
		// Offset the font
		glRasterPos2f( dx*Screen::Info.width, dy*Screen::Info.height );
	}
	else
	{
		// Offset the font
		glTranslatef( dx*Screen::Info.width, dy*Screen::Info.height, 0 );
	}

	// Push Display List Bits
	glPushAttrib(GL_LIST_BIT);
	// Sets The Base Character to the start of the set
	glListBase( fontList - fontInfo.startCharacter );
	// Draws The Display List Text
	glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );
	// Pops The Display List Bits
	glPopAttrib();
	*/
	throw Core::DeprecatedCallException();

	GL.cleanupDraw();
}

void	glDrawing::DrawTextP ( int ix, int iy, const char* fmt, ... )
{
	GL_ACCESS; // Using the glMainSystem accessor
	/*
	return;
	va_list		ap;	// Argument list
	static char	text[256];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	GL.prepareDraw();

	// Change offset behavior based on the font type
	if ( !fontInfo.isTexture )
	{
		// Offset the font
		glRasterPos2i( ix, iy );
	}
	else
	{
		// Offset the font
		glTranslatef( (float)ix, (float)iy, 0 );
	}

	// Push Display List Bits
	glPushAttrib(GL_LIST_BIT);
	// Sets The Base Character to the start of the set
	glListBase( fontList - fontInfo.startCharacter );
	// Draws The Display List Text
	glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );
	// Pops The Display List Bits
	glPopAttrib();
	*/
	throw Core::DeprecatedCallException();

	GL.cleanupDraw();
}

ftype	glDrawing::GetAutoTextWidth ( const char* fmt, ... )
{
	va_list		ap;	// Argument list
	static char	text[1024];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return 0;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	ftype baseScale			 = (Real)CBitmapFont::pActiveFont->GetWidth();
	GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	// begin virtual word draw (curw is cursor)
	int maxlen = strlen( text );
	int lastspot = 0;
	float curw = 0;
	float maxh = fontInfo.height * 1.2f;
	for ( int i = 0; i < maxlen; ++i )
	{ 
		if ( ( ((unsigned char)text[i]) >= fontInfo.startCharacter ) && ( text[i] != '\n' ) )
		{
			int character = text[i]-fontInfo.startCharacter;
			//if (( iPrim2DScalMode == SCALE_WIDTH )||( iPrim2DScalMode == SCALE_DEFAULT )) {
				curw += ( fontInfo.fontOffsets[character].x * Screen::Info.scale );// / ( Screen::Info.width );
			//}
			//else {
			//	curw += ( fontInfo.fontOffsets[character].x * Screen::Info.scale ) / ( Screen::Info.height );
			//}
		}
	}
	if (( iPrim2DScalMode == SCALE_WIDTH )||( iPrim2DScalMode == SCALE_DEFAULT )) {
		return ( curw );// * Screen::Info.width;
	}
	else {
		return ( curw );// * Screen::Info.height;
	}
}

void	glDrawing::DrawAutoText ( float dx, float dy, const char* fmt, ... )
{
	GL_ACCESS GLd_ACCESS

	va_list		ap;	// Argument list
	static char	text[1024];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	ftype baseScale			 = (Real)CBitmapFont::pActiveFont->GetWidth();
	//GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	//GL.prepareDraw();

	// Change offset behavior based on the font type
	/*if ( !fontInfo.isTexture )
	{
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			glRasterPos2f( dx*Screen::Info.width, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			glRasterPos2f( dx*Screen::Info.width, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			glRasterPos2f( dx*Screen::Info.height, dy*Screen::Info.height );
	}
	else*/
	{
		Vector2d translation;
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			translation = Vector2d( dx*Screen::Info.width, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			translation = Vector2d( dx*Screen::Info.width, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			translation = Vector2d( dx*Screen::Info.height, dy*Screen::Info.height );
		// Round position
		if ( iPrim2DRoundMode == RND_ROUND ) {
			translation.x = (float)((int)translation.x);
			translation.y = (float)((int)translation.y);
		}

		// Scale the text by the screen scale value
		//glScalef( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 scaleMatrix;
		scaleMatrix.setScale( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 translationMatrix;
		translationMatrix.setTranslation( translation );
		
		GL.Transform( translationMatrix*scaleMatrix );
		glMaterial::current->setShaderConstants(NULL);
	}

	// Push Display List Bits
	/*glPushAttrib(GL_LIST_BIT);
	// Sets The Base Character to the start of the set
	glListBase( fontList - fontInfo.startCharacter );
	// Draws The Display List Text
	glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );
	// Pops The Display List Bits
	glPopAttrib();*/
	// Loop through every character, and draw it if it's in range
	glDisable( GL_CULL_FACE );
	GLd.BeginPrimitive( GL_TRIANGLES );
	P_PushColor( 1,1,1,1 );
	Vector2d pen (0,0);
	int maxLength = strlen( text );
	for ( int c = 0; c < maxLength; ++c )
	{
		int curChar = text[c] - fontInfo.startCharacter;
		// Check that character is in set
		if ( curChar < 0 || curChar >= (int)fontInfo.setLength ) {
			continue;
		}

		Vector2d uvPos = (fontInfo.charPositions[curChar]) / baseScale;
		Vector2d uvSize = (fontInfo.fontSizes[curChar]) / baseScale;

		Vector2d drawPos = pen - fontInfo.fontOrigins[curChar];
		// Round position
		if ( iPrim2DRoundMode == RND_ROUND ) {
			drawPos.x = (float)((int)drawPos.x);
			drawPos.y = (float)((int)drawPos.y);
		}

		// 0
			P_PushTexcoord( uvPos.x,uvPos.y );
			P_PushNormal( -1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y );
		// 1
			P_PushTexcoord( uvPos.x,uvPos.y+uvSize.y );
			P_PushNormal( -1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y+uvSize.y*baseScale );
		// 2
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
			P_PushNormal( 1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );

		// 0
			P_PushTexcoord( uvPos.x,uvPos.y );
			P_PushNormal( -1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y );
		// 2
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
			P_PushNormal( 1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );
		// 3
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y );
			P_PushNormal( 1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y );

		pen += fontInfo.fontOffsets[curChar];
	}
	GLd.EndPrimitive();
	//glEnable( GL_CULL_FACE );

	//GL.cleanupDraw();
	GL.popModelMatrix();
}

void	glDrawing::DrawAutoTextCentered ( float dx, float dy, const char* fmt, ... )
{
	GL_ACCESS GLd_ACCESS
//	return;
	va_list		ap;	// Argument list
	static char	text[1024];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	ftype baseScale			 = (Real)CBitmapFont::pActiveFont->GetWidth();
	GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	//GL.prepareDraw();

	ftype fHalfStringWidth = GetAutoTextWidth( text ) * 0.5f;

	// Change offset behavior based on the font type
	/*if ( !fontInfo.isTexture )
	{
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			glRasterPos2f( dx*Screen::Info.width - fHalfStringWidth, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			glRasterPos2f( dx*Screen::Info.width - fHalfStringWidth, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			glRasterPos2f( dx*Screen::Info.height - fHalfStringWidth, dy*Screen::Info.height );
	}
	else*/
	{
		Vector2d translation;
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			translation = Vector2d( dx*Screen::Info.width - fHalfStringWidth, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			translation = Vector2d( dx*Screen::Info.width - fHalfStringWidth, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			translation = Vector2d( dx*Screen::Info.height - fHalfStringWidth, dy*Screen::Info.height );
		// Round position
		if ( iPrim2DRoundMode == RND_ROUND ) {
			translation.x = (float)((int)translation.x);
			translation.y = (float)((int)translation.y);
		}

		// Scale the text by the screen scale value
		//glScalef( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 scaleMatrix;
		scaleMatrix.setScale( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 translationMatrix;
		translationMatrix.setTranslation( translation );
		
		GL.Transform( translationMatrix*scaleMatrix );
		glMaterial::current->setShaderConstants(NULL);
	}

	// Push Display List Bits
	/*glPushAttrib(GL_LIST_BIT);
	// Sets The Base Character to the start of the set
	glListBase( fontList - fontInfo.startCharacter );
	// Draws The Display List Text
	glCallLists( strlen(text), GL_UNSIGNED_BYTE, text );
	// Pops The Display List Bits
	glPopAttrib();*/
	// Loop through every character, and draw it if it's in range
	glDisable( GL_CULL_FACE );
	GLd.BeginPrimitive( GL_TRIANGLES );
	P_PushColor( 1,1,1,1 );
	Vector2d pen (0,0);
	int maxLength = strlen( text );
	for ( int c = 0; c < maxLength; ++c )
	{
		int curChar = text[c] - fontInfo.startCharacter;
		// Check that character is in set
		if ( curChar < 0 || curChar >= (int)fontInfo.setLength ) {
			continue;
		}

		Vector2d uvPos = (fontInfo.charPositions[curChar]) / baseScale;
		Vector2d uvSize = (fontInfo.fontSizes[curChar]) / baseScale;

		Vector2d drawPos = pen - fontInfo.fontOrigins[curChar];
		// Round position
		if ( iPrim2DRoundMode == RND_ROUND ) {
			drawPos.x = (float)((int)drawPos.x);
			drawPos.y = (float)((int)drawPos.y);
		}

		// 0
			P_PushTexcoord( uvPos.x,uvPos.y );
			P_PushNormal( -1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y );
		// 1
			P_PushTexcoord( uvPos.x,uvPos.y+uvSize.y );
			P_PushNormal( -1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y+uvSize.y*baseScale );
		// 2
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
			P_PushNormal( 1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );

		// 0
			P_PushTexcoord( uvPos.x,uvPos.y );
			P_PushNormal( -1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x, drawPos.y );
		// 2
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
			P_PushNormal( 1,1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );
		// 3
			P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y );
			P_PushNormal( 1,-1,uvSize.y/uvSize.x );
		P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y );

		pen += fontInfo.fontOffsets[curChar];
	}
	GLd.EndPrimitive();
	//glEnable( GL_CULL_FACE );

	//GL.cleanupDraw();
	GL.popModelMatrix();
}

void		glDrawing::DrawAutoTextWrapped ( float dx, float dy, float dw, const char* fmt, ... )
{
	GL_ACCESS GLd_ACCESS
	//return;
	va_list		ap;	// Argument list
	static char text[8192];

	// If there's nothing to do, don't draw
	if ( fmt == NULL )
		return;

	// Parses string and arguments into a final string
	va_start(ap, fmt);
	    vsprintf(text, fmt, ap);
	va_end(ap);

	// Get the font info
	if ( !CBitmapFont::pActiveFont ) return;
	tBitmapFontInfo fontInfo = CBitmapFont::pActiveFont->GetFontInfo();
	ftype baseScale			 = (Real)CBitmapFont::pActiveFont->GetWidth();
	GLuint fontList			 = CBitmapFont::pActiveFont->GetLetterList();

	//GL.prepareDraw();

	// Change offset behavior based on the font type
	/*if ( !fontInfo.isTexture )
	{
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			glRasterPos2f( dx*Screen::Info.width, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			glRasterPos2f( dx*Screen::Info.width, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			glRasterPos2f( dx*Screen::Info.height, dy*Screen::Info.height );
	}
	else
	{
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			glTranslatef( dx*Screen::Info.width, dy*Screen::Info.height, 0 );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			glTranslatef( dx*Screen::Info.width, dy*Screen::Info.width, 0 );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			glTranslatef( dx*Screen::Info.height, dy*Screen::Info.height, 0 );

		// Scale the text by the screen scale value
		glScalef( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
	}*/
	{
		Vector2d translation;
		// Offset the font
		if ( iPrim2DScalMode == SCALE_DEFAULT )
			translation = Vector2d( dx*Screen::Info.width, dy*Screen::Info.height );
		else if ( iPrim2DScalMode == SCALE_WIDTH )
			translation = Vector2d( dx*Screen::Info.width, dy*Screen::Info.width );
		else if ( iPrim2DScalMode == SCALE_HEIGHT )
			translation = Vector2d( dx*Screen::Info.height, dy*Screen::Info.height );
		// Round position
		if ( iPrim2DRoundMode == RND_ROUND ) {
			translation.x = (float)((int)translation.x);
			translation.y = (float)((int)translation.y);
		}

		// Scale the text by the screen scale value
		//glScalef( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 scaleMatrix;
		scaleMatrix.setScale( Screen::Info.scale, Screen::Info.scale, Screen::Info.scale );
		Matrix4x4 translationMatrix;
		translationMatrix.setTranslation( translation );
		
		GL.Transform( translationMatrix*scaleMatrix );
		glMaterial::current->setShaderConstants(NULL);
	}

	// Push Display List Bits
	//glPushAttrib(GL_LIST_BIT);
	// Sets The Base Character to the start of the set
	//glListBase( fontList - fontInfo.startCharacter );

	glDisable( GL_CULL_FACE );
	GLd.BeginPrimitive( GL_TRIANGLES );
	P_PushColor( 1,1,1,1 );
	Vector2d pen (0,0);
	// begin word warp draw
	int maxLength = strlen( text );
	int lastspot = 0;
	float curw = 0;
	float maxh = fontInfo.height * 1.2f;
	for ( int i = 0; i < maxLength; ++i )
	{ 
		if ( ( ((unsigned char)text[i]) >= fontInfo.startCharacter ) && ( text[i] != '\n' ) )
		{
			if (( iPrim2DScalMode == SCALE_WIDTH )||( iPrim2DScalMode == SCALE_DEFAULT )) {
				curw += ( fontInfo.fontOffsets[text[i]-fontInfo.startCharacter].x * Screen::Info.scale ) / ( Screen::Info.width );
			}
			else {
				curw += ( fontInfo.fontOffsets[text[i]-fontInfo.startCharacter].x * Screen::Info.scale ) / ( Screen::Info.height );
			}
		}
		
		// line wrap
		if (( curw > dw )||( i == maxLength-1 )||( text[i] == '\n' ))
		{
			// Draws The Display List Text
			int targetPosition  = 0;
			if (( i == maxLength-1 )&&( text[i] != '\n' ))
				targetPosition = i-lastspot+1;//, GL_UNSIGNED_BYTE, &(text[lastspot]) );
			else
				targetPosition = i-lastspot;
				//glCallLists( i-lastspot, GL_UNSIGNED_BYTE, &(text[lastspot]) );

			// Draw out the actual text
			for ( int c = 0; c < targetPosition; ++c )
			{
				// Draw the string to this point
				int curChar = text[lastspot+c] - fontInfo.startCharacter;
				// Check that character is in set
				if ( curChar < 0 || curChar >= (int)fontInfo.setLength ) {
					continue;
				}

				Vector2d uvPos = (fontInfo.charPositions[curChar]) / baseScale;
				Vector2d uvSize = (fontInfo.fontSizes[curChar]) / baseScale;

				Vector2d drawPos = pen - fontInfo.fontOrigins[curChar];
				// Round position
				if ( iPrim2DRoundMode == RND_ROUND ) {
					drawPos.x = (float)((int)drawPos.x);
					drawPos.y = (float)((int)drawPos.y);
				}

				// 0
					P_PushTexcoord( uvPos.x,uvPos.y );
					P_PushNormal( -1,-1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x, drawPos.y );
				// 1
					P_PushTexcoord( uvPos.x,uvPos.y+uvSize.y );
					P_PushNormal( -1,1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x, drawPos.y+uvSize.y*baseScale );
				// 2
					P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
					P_PushNormal( 1,1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );

				// 0
					P_PushTexcoord( uvPos.x,uvPos.y );
					P_PushNormal( -1,-1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x, drawPos.y );
				// 2
					P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y+uvSize.y );
					P_PushNormal( 1,1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y+uvSize.y*baseScale );
				// 3
					P_PushTexcoord( uvPos.x+uvSize.x,uvPos.y );
					P_PushNormal( 1,-1,uvSize.y/uvSize.x );
				P_AddVertex( drawPos.x+uvSize.x*baseScale, drawPos.y );

				pen += fontInfo.fontOffsets[curChar];
			}
			
			// Now do word wrap
			if (( iPrim2DScalMode == SCALE_WIDTH )||( iPrim2DScalMode == SCALE_DEFAULT )) {
				//glTranslatef( ( -curw / Screen::Info.scale ) * Screen::Info.width, ( maxh * Screen::Info.scale ), 0 );
				pen += Vector2d( ( -curw / Screen::Info.scale ) * Screen::Info.width, ( maxh * Screen::Info.scale ) );
			}
			else {
				//glTranslatef( ( -curw / Screen::Info.scale ) * Screen::Info.height, ( maxh * Screen::Info.scale ), 0 );
				pen += Vector2d( ( -curw / Screen::Info.scale ) * Screen::Info.height, ( maxh * Screen::Info.scale ) );
			}
			
			if ( text[i] == '\n' )
				lastspot = i+1;
			else
				lastspot = i;
			curw = 0;
		}
	}

	GLd.EndPrimitive();
	//glEnable( GL_CULL_FACE );

	//GL.cleanupDraw();
	GL.popModelMatrix();
}

void		glDrawing::DrawSet2DMode ( e2DDrawMode drawMode )
{
	iPrim2DDrawMode = drawMode;
}
void		glDrawing::DrawSet2DScaleMode ( e2DScaleMode drawMode )
{
	iPrim2DScalMode = drawMode;
}
void		glDrawing::DrawSet2DRounding ( e2DRoundingMode roundMode )
{
	iPrim2DRoundMode = roundMode;
}


// Draw an Autorectangle
void		glDrawing::DrawRectangleA ( float x, float y, float w, float h )
{
	GL_ACCESS GLd_ACCESS

	if ( iPrim2DRoundMode == RND_ROUND ) {
		x = (float)((int)x);
		y = (float)((int)y);
		w = (float)((int)w);
		h = (float)((int)h);
	}

	Vector2d pos [4];
	if ( iPrim2DScalMode == SCALE_DEFAULT )
	{
		pos[0] = Vector2d( x*Screen::Info.width, y*Screen::Info.height );
		pos[1] = Vector2d( x*Screen::Info.width, (y+h)*Screen::Info.height );
		pos[2] = Vector2d( (x+w)*Screen::Info.width, (y+h)*Screen::Info.height );
		pos[3] = Vector2d( (x+w)*Screen::Info.width, y*Screen::Info.height );
	}
	else if ( iPrim2DScalMode == SCALE_WIDTH )
	{
		pos[0] = Vector2d( x*Screen::Info.width, y*Screen::Info.width );
		pos[1] = Vector2d( x*Screen::Info.width, (y+h)*Screen::Info.width );
		pos[2] = Vector2d( (x+w)*Screen::Info.width, (y+h)*Screen::Info.width );
		pos[3] = Vector2d( (x+w)*Screen::Info.width, y*Screen::Info.width );
	}
	else if ( iPrim2DScalMode == SCALE_HEIGHT )
	{
		pos[0] = Vector2d( x*Screen::Info.height, y*Screen::Info.height );
		pos[1] = Vector2d( x*Screen::Info.height, (y+h)*Screen::Info.height );
		pos[2] = Vector2d( (x+w)*Screen::Info.height, (y+h)*Screen::Info.height );
		pos[3] = Vector2d( (x+w)*Screen::Info.height, y*Screen::Info.height );
	}

	if ( iPrim2DDrawMode == D2D_WIRE )
	{
		GLd.BeginPrimitive( GL_LINE_STRIP );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( pos[1].x,pos[1].y );
		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( pos[2].x,pos[2].y );
		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( pos[3].x,pos[3].y );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.EndPrimitive();
	}
	else if ( iPrim2DDrawMode == D2D_FLAT )
	{
		GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( pos[1].x,pos[1].y );
		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( pos[3].x,pos[3].y );
		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( pos[2].x,pos[2].y );
		GLd.EndPrimitive();
	}
}
void		glDrawing::DrawRectangleATex ( float x, float y, float w, float h, float tx, float ty, float tw, float th )
{
	GL_ACCESS GLd_ACCESS

	if ( iPrim2DRoundMode == RND_ROUND ) {
		x = (float)((int)x);
		y = (float)((int)y);
		w = (float)((int)w);
		h = (float)((int)h);
	}

	Vector2d pos [4];
	if ( iPrim2DScalMode == SCALE_DEFAULT )
	{
		pos[0] = Vector2d( x*Screen::Info.width, y*Screen::Info.height );
		pos[1] = Vector2d( x*Screen::Info.width, (y+h)*Screen::Info.height );
		pos[2] = Vector2d( (x+w)*Screen::Info.width, (y+h)*Screen::Info.height );
		pos[3] = Vector2d( (x+w)*Screen::Info.width, y*Screen::Info.height );
	}
	else if ( iPrim2DScalMode == SCALE_WIDTH )
	{
		pos[0] = Vector2d( x*Screen::Info.width, y*Screen::Info.width );
		pos[1] = Vector2d( x*Screen::Info.width, (y+h)*Screen::Info.width );
		pos[2] = Vector2d( (x+w)*Screen::Info.width, (y+h)*Screen::Info.width );
		pos[3] = Vector2d( (x+w)*Screen::Info.width, y*Screen::Info.width );
	}
	else if ( iPrim2DScalMode == SCALE_HEIGHT )
	{
		pos[0] = Vector2d( x*Screen::Info.height, y*Screen::Info.height );
		pos[1] = Vector2d( x*Screen::Info.height, (y+h)*Screen::Info.height );
		pos[2] = Vector2d( (x+w)*Screen::Info.height, (y+h)*Screen::Info.height );
		pos[3] = Vector2d( (x+w)*Screen::Info.height, y*Screen::Info.height );
	}

	if ( iPrim2DDrawMode == D2D_WIRE )
	{
		GLd.BeginPrimitive( GL_LINE_STRIP );
		GLd.P_PushTexcoord( tx,ty );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.P_PushTexcoord( tx,ty+th );
		GLd.P_AddVertex( pos[1].x,pos[1].y );
		GLd.P_PushTexcoord( tx+tw,ty+th );
		GLd.P_AddVertex( pos[2].x,pos[2].y );
		GLd.P_PushTexcoord( tx+tw,ty );
		GLd.P_AddVertex( pos[3].x,pos[3].y );
		GLd.P_PushTexcoord( tx,ty );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.EndPrimitive();
	}
	else if ( iPrim2DDrawMode == D2D_FLAT )
	{
		GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushTexcoord( tx,ty );
		GLd.P_AddVertex( pos[0].x,pos[0].y );
		GLd.P_PushTexcoord( tx,ty+th );
		GLd.P_AddVertex( pos[1].x,pos[1].y );
		GLd.P_PushTexcoord( tx+tw,ty );
		GLd.P_AddVertex( pos[3].x,pos[3].y );
		GLd.P_PushTexcoord( tx+tw,ty+th );
		GLd.P_AddVertex( pos[2].x,pos[2].y );
		GLd.EndPrimitive();
	}
}

// Draw an pixel rectangle
void		glDrawing::DrawRectangle( float x, float y, float w, float h )
{
	GL_ACCESS GLd_ACCESS
	if ( iPrim2DDrawMode == D2D_WIRE )
	{
		GLd.BeginPrimitive( GL_LINE_STRIP );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( (Real)x, (Real)y );
		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( (Real)x, (Real)y+h );
		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( (Real)x+w, (Real)y+h );
		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( (Real)x+w, (Real)y );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( (Real)x, (Real)y );
		GLd.EndPrimitive();
	}
	else if ( iPrim2DDrawMode == D2D_FLAT )
	{
		GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( (Real)x, (Real)y );
		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( (Real)x, (Real)y+h );
		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( (Real)x+w, (Real)y );
		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( (Real)x+w, (Real)y+h );
		GLd.EndPrimitive();
	}
}
// Draw an Autocircle
void		glDrawing::DrawCircleA ( float x, float y, float r )
{
	GL_ACCESS GLd_ACCESS
	uint16_t segs = (uint16_t)std::min<float>( std::max<float>( 4, r*64 + 3 ), 64 );
	GLd.BeginPrimitive( GL_LINE_STRIP );
	float percent;
	float dx, dy, dr;
	dx = x*Screen::Info.width;
	dy = y*Screen::Info.height;
	if ( iPrim2DScalMode == SCALE_DEFAULT ) {
		dr = r*std::min<unsigned int>( Screen::Info.width, Screen::Info.height );
	}
	else if ( iPrim2DScalMode == SCALE_WIDTH ) {
		dr = r*Screen::Info.width;
	}
	else if ( iPrim2DScalMode == SCALE_HEIGHT ) {
		dr = r*Screen::Info.height;
	}
	for ( uint16_t i = 0; i <= segs; i++ )
	{
		percent = ((i/(float)(segs))*360.0f);
		GLd.P_AddVertex(
			Real( dx + cos( degtorad( percent ) )*dr ),
			Real( dy + sin( degtorad( percent ) )*dr ) );
	}
	GLd.EndPrimitive();
}
// Draw a pixel circle
void		glDrawing::DrawCircle ( float x, float y, float r )
{
	GL_ACCESS GLd_ACCESS

	uint16_t segs = (uint16_t)std::min<float>( std::max<float>( 4, r*2 + 3 ), 64 );
	GLd.BeginPrimitive( GL_LINE_STRIP );
	float percent;
	for ( uint16_t i = 0; i <= segs; i++ )
	{
		percent = ((i/(float)(segs))*360.0f);
		GLd.P_AddVertex(
			Real( x + cos( degtorad( percent ) )*r ),
			Real( y + sin( degtorad( percent ) )*r ) );
	}
	GLd.EndPrimitive();
}


// Draw an Autoline
void		glDrawing::DrawLineA ( float x1, float y1, float x2, float y2 )
{
	GL_ACCESS GLd_ACCESS
	GLd.BeginPrimitive( GL_LINE_STRIP );
		GLd.P_AddVertex( x1*Screen::Info.width, y1*Screen::Info.height );
		GLd.P_AddVertex( x2*Screen::Info.width, y2*Screen::Info.height );
	GLd.EndPrimitive();
}
// Draw a pixel line
void		glDrawing::DrawLine ( float x1, float y1, float x2, float y2 )
{
	GL_ACCESS GLd_ACCESS
	GLd.BeginPrimitive( GL_LINE_STRIP );
		GLd.P_AddVertex( (Real)x1, (Real)y1 );
		GLd.P_AddVertex( (Real)x2, (Real)y2 );
	GLd.EndPrimitive();
}

// Draw a screen quad
void		glDrawing::DrawScreenQuad ( void )
{
	GL_ACCESS GLd_ACCESS;

	static glHandle vaoQuad = 0;
	static glHandle vboQuad = 0;
	static uchar attribs [16];

	if ( vaoQuad == 0 )
	{
		glGenVertexArrays( 1, &vaoQuad );
		memset( attribs, 0, sizeof(attribs) );
	}
	if ( vboQuad == 0 )
	{
		glGenBuffers( 1, &vboQuad );
		glBindBuffer( GL_ARRAY_BUFFER, vboQuad );

		CModelVertex buffer [4];
		memset(buffer, 0, sizeof(buffer));
		for (CModelVertex& vert : buffer)
		{
			vert.r = 1.0F;
			vert.g = 1.0F;
			vert.b = 1.0F;
			vert.a = 1.0F;
		}

		// Create quad mesh
		buffer[0].x = 1;
		buffer[0].y = 1;
		buffer[0].u = 1;
		buffer[0].v = 1;

		buffer[1].x = -1;
		buffer[1].y = 1;
		buffer[1].u = 0;
		buffer[1].v = 1;

		buffer[2].x = 1;
		buffer[2].y = -1;
		buffer[2].u = 1;
		buffer[2].v = 0;

		buffer[3].x = -1;
		buffer[3].y = -1;
		buffer[3].u = 0;
		buffer[3].v = 0;

		glBufferData( GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW );
	}

	glBindVertexArray( vaoQuad );
	glBindBuffer( GL_ARRAY_BUFFER, vboQuad );

	// Then disable all values in the VAO (this part of the engine is the only place where they get reused)
	for ( uchar i = 0; i < 16; ++i )
	{
		if ( attribs[i] )
		{	// TODO: DO MATCHING WITH CURRENT MATERIAL.
			attribs[i] = false;
			glDisableVertexAttribArray(i);
		}
	}

	// Now, send the material attributes
	glMaterial::current->bindPassAtrribs(glMaterial::current_pass);
	glMaterial::current->setShaderConstants( NULL, false );
	// Mark the enabled attributes
	for ( uchar i = 0; i < 16; ++i )
	{
		attribs[i] = glPass::enabled_attributes[i];
	}

	// Draw the current primitive
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	/*GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
		GLd.P_PushColor( 1,1,1,1 );

		GLd.P_PushTexcoord( 1,1 );
		GLd.P_AddVertex( 1,1 );

		GLd.P_PushTexcoord( 0,1 );
		GLd.P_AddVertex( -1,1 );

		GLd.P_PushTexcoord( 1,0 );
		GLd.P_AddVertex( 1,-1 );

		GLd.P_PushTexcoord( 0,0 );
		GLd.P_AddVertex( -1,-1 );

	GLd.EndPrimitive();*/
}

// Set primitive options
void		glDrawing::SetLineWidth	( const Real width )
{
	glLineWidth( width );
}