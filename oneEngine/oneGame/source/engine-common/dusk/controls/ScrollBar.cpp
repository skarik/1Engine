#include "engine-common/dusk/controls/ScrollBar.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "engine-common/dusk/Dusk.h"
#include "engine-common/dusk/UIRenderer.h"

void dusk::elements::ScrollBar::SetScroll ( float scroll )
{
	m_scrollSize = std::max(0.0F, m_scrollSize);
	ARCORE_ASSERT(scroll >= 0.0F && scroll <= m_scrollSize);
	m_scrollPosition = math::clamp(scroll, 0.0F, m_scrollSize);
}

dusk::elements::ScrollBar::ScrollBar()
	: dusk::Element()
{
	;
}

void dusk::elements::ScrollBar::Update ( const UIStepInfo* stepinfo )
{
	Element::Update(stepinfo);

	if ( !m_isEnabled )
	{
		m_substateButtonUp = SubelementState();
		m_substateButtonDown = SubelementState();
		m_substateBar = SubelementState();
		m_substateScroller = SubelementState();
		m_draggingBar = false;
	}
	else
	{
		if ( m_isMouseIn && m_wasDrawn && !m_draggingBar )
		{
			// Clear active states
			m_substateButtonUp.activated = false;
			m_substateButtonDown.activated = false;
			m_substateBar.activated = false;
			m_substateScroller.activated = false;

			// Update mouse hover states
			m_substateButtonUp.hovered = m_substateButtonUp.rect.Contains(stepinfo->mouse_position);
			m_substateButtonDown.hovered = m_substateButtonDown.rect.Contains(stepinfo->mouse_position);

			m_substateBar.hovered = m_substateBar.rect.Contains(stepinfo->mouse_position);
			m_substateScroller.hovered = m_substateScroller.rect.Contains(stepinfo->mouse_position);
			if (m_substateScroller.hovered)
			{
				m_substateBar.hovered = false;
			}

			// Act on clicks
			if (CInput::MouseDown(CInput::MBLeft))
			{
				if (m_substateScroller.hovered)
				{
					m_draggingBar = true;
					m_substateScroller.activated = true;
					m_referenceMouseDragScroll = m_substateScroller.rect.pos;
					m_referenceMouseDragPosition = stepinfo->mouse_position;
				}
				else if (m_substateBar.hovered)
				{
					m_substateBar.activated = true;
					if ((m_orientation == Orientation::kHorizontal && stepinfo->mouse_position.x > m_substateScroller.rect.pos.x)
						|| (m_orientation == Orientation::kVertical && stepinfo->mouse_position.y > m_substateScroller.rect.pos.y))
					{
						m_scrollPosition = math::clamp(m_scrollPosition + m_viewSize, 0.0F, m_scrollSize);
					}
					else
					{
						m_scrollPosition = math::clamp(m_scrollPosition - m_viewSize, 0.0F, m_scrollSize);
					}
				}
				else if (m_substateButtonUp.hovered)
				{
					m_substateButtonUp.activated = true;
					m_scrollPosition = math::clamp(m_scrollPosition - 10.0F, 0.0F, m_scrollSize);
				}
				else if (m_substateButtonDown.hovered)
				{
					m_substateButtonDown.activated = true;
					m_scrollPosition = math::clamp(m_scrollPosition + 10.0F, 0.0F, m_scrollSize);
				}
			}
		}
		else if ( m_draggingBar )
		{
			if (!CInput::Mouse(CInput::MBLeft))
			{
				m_draggingBar = false;
			}
			else
			{
				// Update the rect based on the mouse position
				if (m_orientation == Orientation::kHorizontal)
				{
					m_substateScroller.rect.pos.x = m_referenceMouseDragScroll.x + (stepinfo->mouse_position.x - m_referenceMouseDragPosition.x);
					// Get the percent and apply new scroll from there.
					float l_percent = (m_substateScroller.rect.pos.x - m_substateBar.rect.pos.x) / (m_substateBar.rect.size.x - m_substateScroller.rect.size.x);
					m_scrollPosition = math::clamp(l_percent * m_scrollSize, 0.0F, m_scrollSize);
				}
				else
				{
					m_substateScroller.rect.pos.y = m_referenceMouseDragScroll.y + (stepinfo->mouse_position.y - m_referenceMouseDragPosition.y);
					// Get the percent and apply new scroll from there.
					float l_percent = (m_substateScroller.rect.pos.y - m_substateBar.rect.pos.y) / (m_substateBar.rect.size.y - m_substateScroller.rect.size.y);
					m_scrollPosition = math::clamp(l_percent * m_scrollSize, 0.0F, m_scrollSize);
				}
			}
		}
		else
		{
			m_substateButtonUp = SubelementState();
			m_substateButtonDown = SubelementState();
			m_substateBar = SubelementState();
			m_substateScroller = SubelementState();
		}
	}

	m_isActivated = false;
}

void dusk::elements::ScrollBar::Render ( UIRendererContext* uir )
{
	uir->setFocus(dusk::kFocusStyleAutomaticNoHover);
	uir->setColor(dusk::kColorStyleElement);
	uir->drawRectangle(this, m_absoluteRect);

	enum class SubElement
	{
		kButtonUp,
		kButtonDown,
		kScroller,
	};
	SubElement elements [3] = {SubElement::kButtonUp, SubElement::kScroller, SubElement::kButtonDown};
	if (m_buttonPosition == ButtonPosition::kBothAtStart)
	{
		elements[0] = SubElement::kButtonUp;
		elements[1] = SubElement::kButtonDown;
		elements[2] = SubElement::kScroller;
	}
	else if (m_buttonPosition == ButtonPosition::kBothAtEnd)
	{
		elements[0] = SubElement::kScroller;
		elements[1] = SubElement::kButtonUp;
		elements[2] = SubElement::kButtonDown;
	}

	// Loop through and draw the elements:
	Vector2f l_drawPosition = m_absoluteRect.pos;
	for (uint subelementIndex = 0; subelementIndex < 3; ++subelementIndex)
	{
		SubelementState* mainSubelementState = NULL;
		if (elements[subelementIndex] == SubElement::kButtonUp
			|| elements[subelementIndex] == SubElement::kButtonDown)
		{
			mainSubelementState = (elements[subelementIndex] == SubElement::kButtonUp) ? &m_substateButtonUp : &m_substateButtonDown;

			mainSubelementState->rect = Rect(l_drawPosition,
											 Vector2f((m_orientation == Orientation::kHorizontal) ? m_buttonLengthInPixels : m_absoluteRect.size.x,
														(m_orientation == Orientation::kVertical) ? m_buttonLengthInPixels : m_absoluteRect.size.y));

			m_isActivated = mainSubelementState->activated;

			uir->setFocus(mainSubelementState->hovered ? dusk::kFocusStyleHovered : dusk::kFocusStyleAutomaticNoHover);
			uir->setColor(dusk::kColorStyleElementEmphasized, subelementIndex + 1);
			uir->drawRectangle(this, mainSubelementState->rect);
		}
		else if (elements[subelementIndex] == SubElement::kScroller)
		{
			mainSubelementState = &m_substateBar;

			m_substateBar.rect = Rect(l_drawPosition,
									  Vector2f((m_orientation == Orientation::kHorizontal) ? (m_absoluteRect.size.x - m_buttonLengthInPixels * 2.0F) : m_absoluteRect.size.x,
												(m_orientation == Orientation::kVertical) ? (m_absoluteRect.size.y - m_buttonLengthInPixels * 2.0F) : m_absoluteRect.size.y));

			m_substateScroller.rect.size = Vector2f(
				(m_orientation == Orientation::kHorizontal) ? math::clamp(m_viewSize / (m_scrollSize + m_viewSize) * m_substateBar.rect.size.x, 10.0F, m_substateBar.rect.size.x - 0.01F) : m_absoluteRect.size.x - 6.0F,
				  (m_orientation == Orientation::kVertical) ? math::clamp(m_viewSize / (m_scrollSize + m_viewSize) * m_substateBar.rect.size.y, 10.0F, m_substateBar.rect.size.y - 0.01F) : m_absoluteRect.size.y - 6.0F);
			m_substateScroller.rect.pos = l_drawPosition + Vector2f(
				(m_orientation == Orientation::kHorizontal) ? ((m_substateBar.rect.size.x - m_substateScroller.rect.size.x) * (m_scrollPosition / m_scrollSize)) : 3.0F,
				  (m_orientation == Orientation::kVertical) ? ((m_substateBar.rect.size.y - m_substateScroller.rect.size.y) * (m_scrollPosition / m_scrollSize)) : 3.0F);

			// Draw the full bar
			m_isActivated = mainSubelementState->activated;
			uir->setFocus(mainSubelementState->hovered ? dusk::kFocusStyleHovered : dusk::kFocusStyleAutomaticNoHover);
			uir->setColor(dusk::kColorStyleElementEmphasized, subelementIndex + 1);
			uir->drawRectangle(this, mainSubelementState->rect);

			// Now draw the sub-bar for the current position
			m_isActivated = m_substateScroller.activated;
			uir->setFocus(m_substateScroller.hovered ? dusk::kFocusStyleHovered : dusk::kFocusStyleAutomaticNoHover);
			uir->setColor(dusk::kColorStyleElementEmphasized2, 4);
			uir->drawRectangle(this, m_substateScroller.rect);
		}

		if (m_orientation == Orientation::kVertical)
		{
			l_drawPosition.y += mainSubelementState->rect.size.y;
		}
		else
		{
			l_drawPosition.x += mainSubelementState->rect.size.x;
		}
	}
}