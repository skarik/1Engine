#ifndef ENGINE_COMMON_DUSK_UI_LAYOUT_VERTICAL_H_
#define ENGINE_COMMON_DUSK_UI_LAYOUT_VERTICAL_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace layouts {

	class Vertical : public dusk::LayoutElement
	{
	public:
		ENGCOM_API explicit		Vertical ( void ) 
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
				t_currentPosition.y += child->m_absoluteRect.size.y + m_padding.y;
			}
		}

	public:
		Vector2f			m_margin;
		Vector2f			m_padding;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_VERTICAL_H_