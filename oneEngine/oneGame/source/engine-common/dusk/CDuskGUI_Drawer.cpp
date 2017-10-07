
// Include Header
#include "core/time.h"
#include "core/math/Math.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"
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
	Rect draw_rect = rect;

	// Round rect coordinates
	{
		Vector2d min = draw_rect.pos + parenting_offset;
		Vector2d max = draw_rect.pos + draw_rect.size + parenting_offset;

		min.x = (Real)math::round(min.x);
		min.y = (Real)math::round(min.y);
		max.x = (Real)math::round(max.x);
		max.y = (Real)math::round(max.y);

		draw_rect = Rect(min, max-min);
	}

	SetDrawColor();

	m_builder->addRect(
		draw_rect,
		m_drawcolor,
		false);
}
void CDuskGUI::drawRectWire ( const Rect& rect, bool focused )
{
	Rect draw_rect = rect;

	// Round rect coordinates
	{
		Vector2d min = draw_rect.pos + parenting_offset;
		Vector2d max = draw_rect.pos + draw_rect.size + parenting_offset;

		min.x = (Real)math::round(min.x);
		min.y = (Real)math::round(min.y);
		max.x = (Real)math::round(max.x);
		max.y = (Real)math::round(max.y);

		draw_rect = Rect(min, max-min);
	}

	//arModelVertex vert;
	SetDrawColor();
	if ( focused ) {
		m_drawcolor.red *= 1.61F;
		m_drawcolor.green *= 1.61F;
		m_drawcolor.blue *= 1.61F;
	}
	else {
		m_drawcolor.red /= 1.61F * 1.61F;
		m_drawcolor.green /= 1.61F * 1.61F;
		m_drawcolor.blue /= 1.61F * 1.61F;
	}

	m_builder->addRect(
		draw_rect,
		m_drawcolor,
		true);
}
void CDuskGUI::drawLine ( const Real x1, const Real y1, const Real x2, const Real y2 )
{
	SetDrawColor();

	m_builder->addLine(
		Vector2d(x1 + parenting_offset.x, y1 + parenting_offset.y),
		Vector2d(x2 + parenting_offset.x, y2 + parenting_offset.y),
		m_drawcolor);
}

void CDuskGUI::drawText ( const Real x, const Real y, const char* str )
{
	Vector2d draw_pos = Vector2d(x + parenting_offset.x, y + parenting_offset.y);
	draw_pos.x = (Real)math::round(draw_pos.x);
	draw_pos.y = (Real)math::round(draw_pos.y);

	m_builder->addText(
		draw_pos,
		Color(1.0F,1.0F,1.0F,1.0F),
		str);

	//textRequest_t req;
	//req.text = str;
	//req.position.x = x + parenting_offset.x;
	//req.position.y = y + parenting_offset.y;
	//req.mode = 0;
	//if ( bInPixelMode )
	//{
	//	req.position.x /= Screen::Info.width;
	//	req.position.y /= Screen::Info.height;
	//}
	//modelTextRequestList.push_back( req );

	//{
	//	//GL.prepareDraw();
	//	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//	//GL.beginOrtho();
	//	GLd.DrawSet2DRounding( glDrawing::RND_ROUND );

	//	// Draw the text
	//	matFont->setTexture( TEX_MAIN, fntDefault );
	//	matFont->bindPass(0);
	//	//matFont->setShaderConstants( this );
	//	if ( !modelTextRequestList.empty() ) {
	//		for ( uint i = 0; i < modelTextRequestList.size(); ++i )
	//		{
	//			if ( modelTextRequestList[i].mode == 0 ) {
	//				GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 1 ) {
	//				GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 2 ) {
	//				GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
	//			}
	//		}
	//	}
	//	modelTextRequestList.clear();

	//	//GL.endOrtho();
	//	//GL.cleanupDraw();
	//}
}
void CDuskGUI::drawTextWidth ( const Real x, const Real y, const Real w, const char* str )
{
	Vector2d draw_pos = Vector2d(x + parenting_offset.x, y + parenting_offset.y);
	draw_pos.x = (Real)math::round(draw_pos.x);
	draw_pos.y = (Real)math::round(draw_pos.y);

	m_builder->addText(
		draw_pos,
		Color(1.0F,1.0F,1.0F,1.0F),
		str);

	//GL_ACCESS GLd_ACCESS

	//textRequest_t req;
	//req.text = str;
	//req.position.x = x + parenting_offset.x;
	//req.position.y = y + parenting_offset.y;
	//req.mode = 2;
	//req.width = w;
	//if ( bInPixelMode )
	//{
	//	req.position.x /= Screen::Info.width;
	//	req.position.y /= Screen::Info.height;
	//}
	//modelTextRequestList.push_back( req );

	//{
	//	//GL.prepareDraw();
	//	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//	//GL.beginOrtho();
	//	GLd.DrawSet2DRounding( glDrawing::RND_ROUND );

	//	// Draw the text
	//	matFont->setTexture( TEX_MAIN, fntDefault );
	//	matFont->bindPass(0);
	//	//matFont->setShaderConstants( this );
	//	if ( !modelTextRequestList.empty() ) {
	//		for ( uint i = 0; i < modelTextRequestList.size(); ++i )
	//		{
	//			if ( modelTextRequestList[i].mode == 0 ) {
	//				GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 1 ) {
	//				GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 2 ) {
	//				GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
	//			}
	//		}
	//	}
	//	modelTextRequestList.clear();

	//	//GL.endOrtho();
	//	//GL.cleanupDraw();
	//}
}
void CDuskGUI::drawTextCentered ( const Real x, const Real y, const char* str )
{
	Vector2d draw_pos = Vector2d(x + parenting_offset.x, y + parenting_offset.y);
	draw_pos.x = (Real)math::round(draw_pos.x);
	draw_pos.y = (Real)math::round(draw_pos.y);

	m_builder->addText(
		draw_pos,
		Color(1.0F,1.0F,1.0F,1.0F),
		str);

	//GL_ACCESS GLd_ACCESS

	//textRequest_t req;
	//req.text = str;
	//req.position.x = x + parenting_offset.x;
	//req.position.y = y + parenting_offset.y;
	//req.mode = 1;
	//if ( bInPixelMode )
	//{
	//	req.position.x /= Screen::Info.width;
	//	req.position.y /= Screen::Info.height;
	//}
	//modelTextRequestList.push_back( req );

	//{
	//	//GL.prepareDraw();
	//	core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
	//	//GL.beginOrtho();
	//	GLd.DrawSet2DRounding( glDrawing::RND_ROUND );

	//	// Draw the text
	//	matFont->setTexture( TEX_MAIN, fntDefault );
	//	matFont->bindPass(0);
	//	//matFont->setShaderConstants( this );
	//	if ( !modelTextRequestList.empty() ) {
	//		for ( uint i = 0; i < modelTextRequestList.size(); ++i )
	//		{
	//			if ( modelTextRequestList[i].mode == 0 ) {
	//				GLd.DrawAutoText( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 1 ) {
	//				GLd.DrawAutoTextCentered( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].text.c_str() );
	//			}
	//			else if ( modelTextRequestList[i].mode == 2 ) {
	//				GLd.DrawAutoTextWrapped( modelTextRequestList[i].position.x, modelTextRequestList[i].position.y, modelTextRequestList[i].width, modelTextRequestList[i].text.c_str() );
	//			}
	//		}
	//	}
	//	modelTextRequestList.clear();

	//	//GL.endOrtho();
	//	//GL.cleanupDraw();
	//}
}