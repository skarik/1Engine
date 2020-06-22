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

}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_HORIZONTAL_H_