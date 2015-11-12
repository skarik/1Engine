
// Include Header
#include "core/time.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "CDuskGUI.h"

void CDuskGUI::setDrawDown ( void )
{
	mainColorMode = 2;
}
void CDuskGUI::setDrawHover ( void )
{
	mainColorMode = 1;
}
void CDuskGUI::setDrawDefault ( void )
{
	mainColorMode = 0;
}

void CDuskGUI::setSubdrawSelection ( void )
{
	drawDark = false;
	drawLight = true;
}
void CDuskGUI::setSubdrawDarkSelection ( void )
{
	drawDark = true;
	drawLight = false;
}

void CDuskGUI::setSubdrawTransparent ( void )
{
	drawOpacity = 1;
}
void CDuskGUI::setSubdrawOpaque ( void )
{
	drawOpacity = 2;
}

void CDuskGUI::setSubdrawNotice ( void )
{
	drawError = 1;
}
void CDuskGUI::setSubdrawError ( void )
{
	drawError = 2;
}

void CDuskGUI::setSubdrawPulse ( void )
{
	drawPulse = true;
}
void CDuskGUI::setSubdrawOverrideColor ( const Color& color )
{
	m_drawcolor = color;
	drawColorOverride = true;
}

void CDuskGUI::setSubdrawDefault ( void )
{
	drawDark = false;
	drawLight = false;
	drawOpacity = 0;
	drawError = 0;
	drawPulse = false;
	drawColorOverride = false;
}

void CDuskGUI::SetDrawColor ( void )
{
	if ( !drawColorOverride )
	{
		switch ( mainColorMode ) {
		case 0:
			m_drawcolor = m_basecolor;
			break;
		case 1:
			m_drawcolor = m_basecolor_hover;
			break;
		case 2:
			m_drawcolor = m_basecolor_down;
			break;
		}
		if ( drawError == 1 ) {
			m_drawcolor.red *= 1.4f;
			m_drawcolor.green *= 1.4f;
			m_drawcolor.red = (m_drawcolor.red+m_drawcolor.green)/2;
			m_drawcolor.red = m_drawcolor.green;
			m_drawcolor.blue *= 0.1f;
		}
		else if ( drawError == 2 ) {
			m_drawcolor.red *= 1.5f;
			m_drawcolor.green *= 0.1f;
			m_drawcolor.blue *= 0.1f;
		}
		if ( drawDark ) {
			m_drawcolor.red *= 0.6f;
			m_drawcolor.green *= 0.6f;
			m_drawcolor.blue *= 0.6f;
		}
		else if ( drawLight ) {
			m_drawcolor.red *= 1.3f;
			m_drawcolor.green *= 1.3f;
			m_drawcolor.blue *= 1.3f;
		}
		if ( drawPulse ) {
			m_drawcolor.alpha = sin( Time::currentTime * 3.0f )*0.5f + 0.5f;
		}
		if ( drawOpacity == 1 ) {
			m_drawcolor.alpha *= 0.6f;
		}
		else if ( drawOpacity == 2 ) {
			m_drawcolor.alpha *= 1.6f;
		}
	}
	else {
		if ( drawPulse ) {
			m_drawcolor.alpha = m_drawcolor.alpha * (sin( Time::currentTime * 3.0f )*0.5f + 0.5f);
		}
	}
}


void CDuskGUI::drawRect ( const Rect& rect )
{
	GL_ACCESS GLd_ACCESS

	CModelVertex vert;
	SetDrawColor();
	vert.r = m_drawcolor.red;
	vert.g = m_drawcolor.green;
	vert.b = m_drawcolor.blue;
	vert.a = m_drawcolor.alpha;
	vert.u = 0.1f;
	vert.v = 0.1f;
	vert.z = 0;

	vert.x = rect.pos.x;
	vert.y = rect.pos.y;
	modelSolidMeshList.push_back( vert );
	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y;
	modelSolidMeshList.push_back( vert );
	vert.x = rect.pos.x;
	vert.y = rect.pos.y+rect.size.y;
	modelSolidMeshList.push_back( vert );

	vert.x = rect.pos.x;
	vert.y = rect.pos.y+rect.size.y;
	modelSolidMeshList.push_back( vert );
	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y;
	modelSolidMeshList.push_back( vert );
	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y+rect.size.y;
	modelSolidMeshList.push_back( vert );

	{
		GL.prepareDraw();

		GL.beginOrtho( 0,0, 1,1, -45,45 );
		GLd.DrawSet2DScaleMode();

		matDefault->bindPass(0);
		matDefault->setShaderConstants( this );

		GLd.BeginPrimitive( GL_TRIANGLES );
		for ( uint i = 0; i < modelSolidMeshList.size(); ++i )
		{
			GLd.P_PushColor( modelSolidMeshList[i].r, modelSolidMeshList[i].g, modelSolidMeshList[i].b, modelSolidMeshList[i].a );
			GLd.P_AddVertex( modelSolidMeshList[i].x, modelSolidMeshList[i].y );
		}
		GLd.EndPrimitive();
		modelSolidMeshList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}
void CDuskGUI::drawRectWire ( const Rect& rect, bool focused )
{
	GL_ACCESS GLd_ACCESS

	CModelVertex vert;
	SetDrawColor();
	if ( focused ) {
		m_drawcolor.red *= 1.1f;
		m_drawcolor.green *= 1.1f;
		m_drawcolor.blue *= 1.1f;
	}
	else {
		m_drawcolor.red *= 0.6f;
		m_drawcolor.green *= 0.6f;
		m_drawcolor.blue *= 0.6f;
	}
	vert.r = m_drawcolor.red;
	vert.g = m_drawcolor.green;
	vert.b = m_drawcolor.blue;
	vert.a = m_drawcolor.alpha;
	vert.u = 0.1f;
	vert.v = 0.1f;
	vert.z = 0;

	vert.x = rect.pos.x;
	vert.y = rect.pos.y;
	modelLineMeshList.push_back( vert );
	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y;
	modelLineMeshList.push_back( vert );

	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y;
	modelLineMeshList.push_back( vert );
	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y+rect.size.y;
	modelLineMeshList.push_back( vert );

	vert.x = rect.pos.x+rect.size.x;
	vert.y = rect.pos.y+rect.size.y;
	modelLineMeshList.push_back( vert );
	vert.x = rect.pos.x;
	vert.y = rect.pos.y+rect.size.y;
	modelLineMeshList.push_back( vert );

	vert.x = rect.pos.x;
	vert.y = rect.pos.y+rect.size.y;
	modelLineMeshList.push_back( vert );
	vert.x = rect.pos.x;
	vert.y = rect.pos.y;
	modelLineMeshList.push_back( vert );

	{
		GL.prepareDraw();

		GL.beginOrtho( 0,0, 1,1, -45,45 );
		GLd.DrawSet2DScaleMode();

		matDefault->bindPass(0);
		matDefault->setShaderConstants( this );
		GLd.BeginPrimitive( GL_LINES );
		for ( uint i = 0; i < modelLineMeshList.size(); ++i )
		{
			GLd.P_PushColor( modelLineMeshList[i].r, modelLineMeshList[i].g, modelLineMeshList[i].b, modelLineMeshList[i].a );
			GLd.P_AddVertex( modelLineMeshList[i].x, modelLineMeshList[i].y );
		}
		GLd.EndPrimitive();
		modelLineMeshList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}
void CDuskGUI::drawLine ( const ftype x1, const ftype y1, const ftype x2, const ftype y2 )
{
	GL_ACCESS GLd_ACCESS

	CModelVertex vert;
	SetDrawColor();
	vert.r = m_drawcolor.red;
	vert.g = m_drawcolor.green;
	vert.b = m_drawcolor.blue;
	vert.a = m_drawcolor.alpha;
	vert.u = 0.1f;
	vert.v = 0.1f;
	vert.z = 0;

	vert.x = x1;
	vert.y = y1;
	modelLineMeshList.push_back( vert );
	vert.x = x2;
	vert.y = y2;
	modelLineMeshList.push_back( vert );

	{
		GL.prepareDraw();

		GL.beginOrtho( 0,0, 1,1, -45,45 );
		GLd.DrawSet2DScaleMode();

		matDefault->bindPass(0);
		matDefault->setShaderConstants( this );
		GLd.BeginPrimitive( GL_LINES );
		for ( uint i = 0; i < modelLineMeshList.size(); ++i )
		{
			GLd.P_PushColor( modelLineMeshList[i].r, modelLineMeshList[i].g, modelLineMeshList[i].b, modelLineMeshList[i].a );
			GLd.P_AddVertex( modelLineMeshList[i].x, modelLineMeshList[i].y );
		}
		GLd.EndPrimitive();
		modelLineMeshList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}

void CDuskGUI::drawText ( const ftype x, const ftype y, const char* str )
{
	GL_ACCESS GLd_ACCESS

	textRequest_t req;
	req.text = str;
	req.position.x = x;
	req.position.y = y;
	req.mode = 0;
	modelTextRequestList.push_back( req );

	{
		GL.prepareDraw();
		GL.beginOrtho();

		// Draw the text
		matFont->setTexture( 0, fntDefault );
		matFont->bindPass(0);
		matFont->setShaderConstants( this );
		if ( !modelTextRequestList.empty() ) {
			for ( uint i = 0; i < modelTextRequestList.size(); ++i )
			{
				if ( modelTextRequestList[i].mode == 0 ) {
					GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 1 ) {
					GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 2 ) {
					GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
				}
			}
		}
		modelTextRequestList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}
void CDuskGUI::drawTextWidth ( const ftype x, const ftype y, const ftype w, const char* str )
{
	GL_ACCESS GLd_ACCESS

	textRequest_t req;
	req.text = str;
	req.position.x = x;
	req.position.y = y;
	req.mode = 2;
	req.width = w;
	modelTextRequestList.push_back( req );

	{
		GL.prepareDraw();
		GL.beginOrtho();

		// Draw the text
		matFont->setTexture( 0, fntDefault );
		matFont->bindPass(0);
		matFont->setShaderConstants( this );
		if ( !modelTextRequestList.empty() ) {
			for ( uint i = 0; i < modelTextRequestList.size(); ++i )
			{
				if ( modelTextRequestList[i].mode == 0 ) {
					GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 1 ) {
					GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 2 ) {
					GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
				}
			}
		}
		modelTextRequestList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}
void CDuskGUI::drawTextCentered ( const ftype x, const ftype y, const char* str )
{
	GL_ACCESS GLd_ACCESS

	textRequest_t req;
	req.text = str;
	req.position.x = x;
	req.position.y = y;
	req.mode = 1;
	modelTextRequestList.push_back( req );

	{
		GL.prepareDraw();
		GL.beginOrtho();

		// Draw the text
		matFont->setTexture( 0, fntDefault );
		matFont->bindPass(0);
		matFont->setShaderConstants( this );
		if ( !modelTextRequestList.empty() ) {
			for ( uint i = 0; i < modelTextRequestList.size(); ++i )
			{
				if ( modelTextRequestList[i].mode == 0 ) {
					GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 1 ) {
					GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
				}
				else if ( modelTextRequestList[i].mode == 2 ) {
					GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
				}
			}
		}
		modelTextRequestList.clear();

		GL.endOrtho();
		GL.cleanupDraw();
	}
}