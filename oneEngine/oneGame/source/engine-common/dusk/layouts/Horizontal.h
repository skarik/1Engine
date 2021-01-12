#ifndef ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_
#define ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_

#include "engine-common/dusk/Element.h"

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

	enum class AlignStyleHorizontal
	{
		kLeft,
		kCenter,
		kRight,
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

			/*Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
			for (Element* child : elements)
			{
				child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
				t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x;
			}*/

			// Get total width of the items to place.
			float availableWidth = m_absoluteRect.size.x - m_margin.x * 2.0F;
			float totalWidth = 0.0;
			for (Element* child : elements)
			{
				// Add to total width
				totalWidth += child->m_absoluteRect.size.x;
				// Save the largest height so we can size the fitter properly
				m_localRect.size.y = std::max<float>(m_localRect.size.y, child->m_absoluteRect.size.y);
			}
			totalWidth += (elements.size() - 1) * m_padding.x;

			// Update the height now
			m_localRect.size.y += m_margin.y * 2.0F;
			m_absoluteRect.size.y = m_localRect.size.y;

			// Calculate the extra space, and the amount we can add total to padd
			float availablePadding = std::max<float>(0.0F, availableWidth - totalWidth);
			float paddingExpansion = m_spaceItems ? (availablePadding / (elements.size() - 1)) : 0.0F;

			// Place the items now
			if (m_spaceItems)
			{	// If equally spaced, justification has no meaning.
				Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
				for (Element* child : elements)
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x + paddingExpansion;
				}
			}
			else
			{
				Vector2f t_currentPosition = m_absoluteRect.pos + m_margin;
				if (m_justify == AlignStyleHorizontal::kLeft)
					{}
				else if (m_justify == AlignStyleHorizontal::kRight)
				{
					t_currentPosition.x += availableWidth - totalWidth;
				}
				else if (m_justify == AlignStyleHorizontal::kCenter)
				{
					t_currentPosition.x += availableWidth - totalWidth * 0.5F;
				}

				for (Element* child : elements)
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					t_currentPosition.x += child->m_absoluteRect.size.x + m_padding.x;
				}
			}
		}

	public:
		AlignStyleHorizontal
							m_justify = AlignStyleHorizontal::kLeft;
		bool				m_spaceItems = false;
		Vector2f			m_margin;
		Vector2f			m_padding;
	};
}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_