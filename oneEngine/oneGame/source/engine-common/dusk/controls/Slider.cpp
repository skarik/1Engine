#include "Slider.h"

#include "core/input/CInput.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"
#include "core/math/Math.h"

template <typename NumberType>
void dusk::elements::Slider<NumberType>::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	if ( !m_isEnabled )
	{
		//isPressed = false;
	}
	else
	{
		const NumberType l_previousValue = m_value;

		if ( m_isMouseIn && m_wasDrawn )
		{
			// Mouse controls
			if ( core::Input::Mouse(core::kMBLeft) )
			{
				// Check the mouse position versus the absolute rect
				Real32 mousePercent = math::saturate<Real32>((core::Input::MouseX() - (m_absoluteRect.pos.x + 2.0F)) / (m_absoluteRect.size.x - 4.0F));

				// And we apply the limit
				m_value = (NumberType)math::lerp<Real32>(mousePercent, (Real32)m_range_min, (Real32)m_range_max);

				// Then we snap
				if (m_snap)
				{
					m_value = (NumberType)(math::round<Real32>((Real32)((Real32)m_value / m_snap_divisor)) * m_snap_divisor);
					// And limit again
					m_value = (NumberType)math::clamp<NumberType>(m_value, m_range_min, m_range_max);
				}
			}
		}
		else
		{
			// Keyboard controls
			if ( m_isFocused )
			{
				if ( core::Input::Keydown( core::kVkReturn ) )
				{
				}
				else if ( core::Input::Keydown( core::kVkLeft ) )
				{
					m_value += m_snap ? m_snap_divisor : (NumberType)((m_range_max - m_range_min) * 0.1F);
					m_value = (NumberType)math::clamp(m_value, m_range_min, m_range_max);
				}
				else if ( core::Input::Keydown( core::kVkRight ) )
				{
					m_value -= m_snap ? m_snap_divisor : (NumberType)((m_range_max - m_range_min) * 0.1F);
					m_value = (NumberType)math::clamp(m_value, m_range_min, m_range_max);
				}
			}
		}

		// Update change callbacks
		if (l_previousValue != m_value && m_onValueChange != nullptr)
		{
			m_onValueChange(m_value);
		}
	}

	m_isActivated = false;
}

template <typename NumberType>
void dusk::elements::Slider<NumberType>::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomatic);
	
	// Draw the center line rect
	const Real32 centerLineHalfWidth = 2.0F;
	Vector3f center = m_absoluteRect.pos + m_absoluteRect.size * 0.5F;
	Rect centerLineRect(Vector2f(m_absoluteRect.pos.x, center.y - centerLineHalfWidth), Vector2f(m_absoluteRect.size.x, centerLineHalfWidth * 2.0F));
	uir->setColor(dusk::kColorStyleShapeNormal);
	uir->drawRectangle(this, centerLineRect);

	// Draw the selection cursor
	const Real32 cursorRectWidth = 4.0F;
	const Real32 cursorRectHalfHeight = std::min<Real32>(m_absoluteRect.size.y * 0.5F, 8.0F);
	const Real32 cursorParametricPosition = math::saturate((Real32)(m_value - m_range_min) / (Real32)(m_range_max - m_range_min));
	Rect cursorRect(
		Vector2f(m_absoluteRect.pos.x + cursorParametricPosition * (m_absoluteRect.size.x - cursorRectWidth), center.y - cursorRectHalfHeight),
		Vector2f(cursorRectWidth, cursorRectHalfHeight * 2)
	);
	uir->setColor(dusk::kColorStyleShapeAccented);
	uir->drawRectangle(this, Rect(centerLineRect.pos.x, centerLineRect.pos.y, centerLineRect.size.x * cursorParametricPosition, centerLineRect.size.y));
	uir->drawRectangle(this, cursorRect);

	// Draw the values at far left and right
	uir->setColor(dusk::kColorStyleLabel);
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignLeft, dusk::kTextAlignTop});
	uir->drawText(this, Vector2f(m_absoluteRect.pos.x, center.y) + Vector2f(5.0F, centerLineHalfWidth * 2.0F), std::to_string(m_range_min).c_str());
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignRight, dusk::kTextAlignTop});
	uir->drawText(this, Vector2f(m_absoluteRect.pos.x, center.y) + Vector2f(m_absoluteRect.size.x - 5.0F, centerLineHalfWidth * 2.0F), std::to_string(m_range_max).c_str());

	// Draw the current value on the top
	uir->setTextSettings(TextStyleSettings{dusk::kTextFontButton, dusk::kTextAlignCenter, dusk::kTextAlignBottom});
	uir->drawText(this, Vector2f(m_absoluteRect.pos.x, center.y) + Vector2f(m_absoluteRect.size.x * 0.5F, -centerLineHalfWidth * 2.0F), std::to_string(m_value).c_str());
}

template class dusk::elements::Slider<float>;
template class dusk::elements::Slider<double>;
template class dusk::elements::Slider<int32_t>;