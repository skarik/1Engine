#include "UIRenderer.h"

#include "engine-common/dusk/Element.h"

void dusk::UIRendererContext::setFocus ( FocusStyle style )
{
	m_focusType = style;
}
void dusk::UIRendererContext::setColor ( ColorStyle style )
{
	m_colorType = style;
}
void dusk::UIRendererContext::setTextSettings ( TextStyleSettings settings )
{
	m_textType = settings;
}


void dusk::UIRendererContext::generateColor( Element* source )
{
	const Color kAccentColor (0.16F, 0.32F, 0.96F, 1.00F);
	const Color kBackgroundColor (0.16F, 0.16F, 0.16F, 1.00F);
	const Color kBackgroundBump (0.16F, 0.16F, 0.16F, 1.00F);

	// focused is normal color but adds glow
	// disabled is normal color but gray
	// hovered is brighter color in general

}
void dusk::UIRendererContext::generateDrawState( Element* source )
{

}


float dusk::UIRendererContext::getTextHeight ( TextFontStyle font )
{
	return 0;
}

void dusk::UIRendererContext::drawRectangle ( Element* source, const Rect& rectangle )
{
}
void dusk::UIRendererContext::drawText ( Element* source, const Vector2f& position, const char* str )
{
}