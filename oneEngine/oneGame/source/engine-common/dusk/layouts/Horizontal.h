#ifndef ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_
#define ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/layouts/LayoutCommon.h"

namespace dusk {
namespace layouts {

	class Horizontal : public dusk::LayoutElement
	{
	public:
		ENGCOM_API explicit		Horizontal ( void ) 
			: dusk::LayoutElement()
			{}

		//	LayoutChildren() : Called every frame by the UI system.
		void					LayoutChildren ( std::vector<Element*>& elements ) override
		{
			if (m_parent != NULL)
			{
				if (m_parent->GetElementType() == ElementType::kLayout)
				{
					m_localRect.size = m_parent->m_localRect.size;
					m_absoluteRect.size = m_parent->m_absoluteRect.size;
				}
				else
				{
					m_localRect = m_parent->m_localRect;
					m_absoluteRect = m_parent->m_absoluteRect;
				}
			}

			Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
			for (Element* child : elements)
			{
				child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
				t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x;
			}
		}

	public:
		Vector2f			m_margin;
		Vector2f			m_padding;
	};

	class HorizontalFit : public dusk::LayoutElement
	{
	public:
		ENGCOM_API explicit		HorizontalFit ( void ) 
			: dusk::LayoutElement()
		{}

		//	LayoutChildren() : Called every frame by the UI system.
		void					LayoutChildren ( std::vector<Element*>& elements ) override
		{
			ARCORE_ASSERT(m_parent != NULL);
			if (m_parent->GetElementType() == ElementType::kLayout)
			{
				m_localRect.size = m_parent->m_localRect.size;
				m_absoluteRect.size = m_parent->m_absoluteRect.size;
			}
			else
			{
				m_localRect = m_parent->m_localRect;
				m_absoluteRect = m_parent->m_absoluteRect;
			}
			m_localRect.size.y = 0;

			// Get total width of the items to place.
			float availableWidth = m_absoluteRect.size.x - m_margin.x * 2.0F;
			float availableHeight = m_absoluteRect.size.y - m_margin.y * 2.0F;
			float totalWidth = 0.0;
			float largestHeight = 0.0;
			for (Element* child : elements)
			{
				// Add to total width
				totalWidth += child->m_absoluteRect.size.x;
				// Save the largest height so we can size the fitter properly
				largestHeight = std::max<float>(largestHeight, child->m_absoluteRect.size.y);
			}
			float totalWidthPadding = (elements.size() - 1) * m_padding.x;
			float totalWidthWithPadding = totalWidth + totalWidthPadding;

			// Update the height now
			m_localRect.size.y = largestHeight + m_margin.y * 2.0F;
			//m_absoluteRect.size.y = m_localRect.size.y;

			// Calculate the extra space, and the amount we can add total to padd
			float availablePadding = std::max<float>(0.0F, availableWidth - totalWidthWithPadding);
			float paddingExpansion = (m_justifyScaleStyle == JustifyScaleStyle::kSpaceBetween) ? (availablePadding / (elements.size() - 1)) : 0.0F;

			// Place the items now
			if (m_justifyScaleStyle == JustifyScaleStyle::kStretch)
			{
				// If stretched, we need to rescale the width as we go.
				Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
				for (Element* child : elements)
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					child->m_absoluteRect.size.x = child->m_absoluteRect.size.x / totalWidth * (availableWidth - totalWidthPadding);
					child->m_absoluteRect.size.y = m_fitItemHeight ? (child->m_absoluteRect.size.y / largestHeight * availableHeight) : (child->m_absoluteRect.size.y);
					t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x + paddingExpansion;
				}
			}
			else if (m_justifyScaleStyle == JustifyScaleStyle::kSpaceBetween)
			{	
				// If equally spaced, justification has no meaning.
				Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
				for (Element* child : elements)
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					child->m_absoluteRect.size.y = m_fitItemHeight ? (child->m_absoluteRect.size.y / largestHeight * availableHeight) : (child->m_absoluteRect.size.y);
					t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x + paddingExpansion;
				}
			}
			else // JustifyScaleStyle::kInline
			{	
				// If items aren't spaced, we place them 
				Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
				if (m_justify == AlignStyleHorizontal::kLeft)
					{}
				else if (m_justify == AlignStyleHorizontal::kRight)
				{
					t_currentPosition.x += availableWidth - totalWidthWithPadding;
				}
				else if (m_justify == AlignStyleHorizontal::kCenter)
				{
					t_currentPosition.x += availableWidth - totalWidthWithPadding * 0.5F;
				}

				for (Element* child : elements)
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					child->m_absoluteRect.size.y = m_fitItemHeight ? (child->m_absoluteRect.size.y / largestHeight * availableHeight) : (child->m_absoluteRect.size.y);
					t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x;
				}
			}
		}

	public:
		AlignStyleHorizontal
							m_justify = AlignStyleHorizontal::kLeft;
		JustifyScaleStyle
							m_justifyScaleStyle = JustifyScaleStyle::kInline;
		// Items will be scaled proportional to the total Y-size of items.
		bool				m_fitItemHeight = false;
		Vector2f			m_margin;
		Vector2f			m_padding;
	};
}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_