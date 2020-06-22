#include "UIRenderer.h"

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
	m_mb2->enableAttribute(renderer::shader::kVBufferSlotNormal);
	auto tVertexCount = m_mb2->getModelDataVertexCount();

	m_mb2->addRect(rectangle, Color(0.16F, 0.16F, 0.16F, 1.00F), false);

	// Zero out normals to disable texture use on this shape
	for (uint16_t i = tVertexCount; i < m_mb2->getModelDataVertexCount(); ++i)
	{
		m_modeldata->normal[i] = Vector3f(0.0F, 0.0F, 0.0F);
	}
}
void dusk::UIRendererContext::drawText ( Element* source, const Vector2f& position, const char* str )
{
	static_cast<rrTextBuilder2D*>(m_mb2)->addText(position, Color(1.0F, 1.0F, 1.0F, 1.0F), str);
}