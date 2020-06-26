#include "UIRenderer.h"

#include "core/math/Math.h"
#include "engine-common/dusk/Element.h"
#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

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

	m_dsColorBackground = kBackgroundColor;

	if (m_focusType == kFocusStyleDisabled 
		|| (m_focusType == kFocusStyleAutomatic && !source->m_isEnabled)
		|| (m_focusType == kFocusStyleAutomaticNoHover && !source->m_isEnabled)
		)
	{
		m_dsColorBackground = kBackgroundColor * 0.5F;

		m_dsDrawBackground = true;
		m_dsDrawOutline = false;
	}
	else if (m_focusType == kFocusStyleHovered
		|| (m_focusType == kFocusStyleAutomatic && source->m_isMouseIn)
		)
	{
		m_dsColorBackground = kBackgroundColor + kBackgroundBump;
		m_dsColorBackground.a = kBackgroundColor.a;

		m_dsDrawBackground = true;
		m_dsDrawOutline = false;
	}
	else if (m_focusType == kFocusStyleFocused
		|| (m_focusType == kFocusStyleAutomatic && source->m_isFocused)
		|| (m_focusType == kFocusStyleAutomaticNoHover && source->m_isFocused)
		)
	{
		m_dsColorBackground = kBackgroundColor + kBackgroundBump;
		m_dsColorBackground.a = kBackgroundColor.a;

		m_dsColorOutline = kAccentColor;

		m_dsDrawBackground = true;
		m_dsDrawOutline = true;
	}
	else if (m_focusType == kFocusStyleActive
		|| (m_focusType == kFocusStyleAutomatic && source->m_isEnabled)
		|| (m_focusType == kFocusStyleAutomaticNoHover && source->m_isEnabled)
		)
	{
		m_dsColorBackground = kBackgroundColor + kBackgroundBump;
		m_dsColorBackground.a = kBackgroundColor.a;

		m_dsDrawBackground = true;
		m_dsDrawOutline = false;
	}
}
void dusk::UIRendererContext::generateDrawState( Element* source )
{

}


float dusk::UIRendererContext::getTextHeight ( TextFontStyle font )
{
	return 16; // TODO: Use font setting given by either renderer, or grabbed from renderer
}

void dusk::UIRendererContext::drawRectangle ( Element* source, const Rect& rectangle )
{
	m_mb2->enableAttribute(renderer::shader::kVBufferSlotNormal);
	auto tVertexCount = m_mb2->getModelDataVertexCount();

	generateColor(source);

	if (m_dsDrawBackground)
		m_mb2->addRect(rectangle, m_dsColorBackground, false);
	if (m_dsDrawOutline)
		m_mb2->addRect(rectangle, m_dsColorOutline, true);

	// Zero out normals to disable texture use on this shape
	for (uint16_t i = tVertexCount; i < m_mb2->getModelDataVertexCount(); ++i)
	{
		m_modeldata->normal[i] = Vector3f(0.0F, 0.0F, 0.0F);
	}
}
void dusk::UIRendererContext::drawText ( Element* source, const Vector2f& position, const char* str )
{
	rrTextBuilder2D* l_textBuilder = static_cast<rrTextBuilder2D*>(m_mb2);
	Vector2f l_drawPosition (math::round(position.x), math::round(position.y + 0.9F * getTextHeight(m_textType.font)));

	l_textBuilder->addText(
		l_drawPosition,
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		str);
}