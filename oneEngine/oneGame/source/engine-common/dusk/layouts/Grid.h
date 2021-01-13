#ifndef ENGINE_COMMON_DUSK_UI_LAYOUT_GRID_H_
#define ENGINE_COMMON_DUSK_UI_LAYOUT_GRID_H_

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/layouts/LayoutCommon.h"

namespace dusk {
namespace layouts {

	class StretchGridRow : public dusk::LayoutElement
	{
	public:
		ENGCOM_API explicit		StretchGridRow ( void ) 
			: dusk::LayoutElement()
			{}

		//	LayoutChildren() : Called every frame by the UI system.
		void					LayoutChildren ( std::vector<Element*>& elements ) override
		{
			/*if (m_parent != NULL)
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
			}*/
			m_localRect.size.x = m_parent->m_localRect.size.x;
			m_absoluteRect.size.x = m_parent->m_absoluteRect.size.x;

			float totalWidth = 0.0F;
			float maxHeight = 0.0F;
			for (Element* child : elements)
			{
				totalWidth += child->m_localRect.size.x;
				maxHeight = std::max<float>(maxHeight, child->m_localRect.size.y);
			}

			float heightRatio = m_absoluteRect.size.y / maxHeight;

			Vector2f t_currentPosition = m_absoluteRect.pos;
			if (m_justify == AlignStyleHorizontal::kLeft)
				{}
			else if (m_justify == AlignStyleHorizontal::kRight)
			{
				t_currentPosition.x += m_absoluteRect.size.x - totalWidth;
			}
			else if (m_justify == AlignStyleHorizontal::kCenter)
			{
				t_currentPosition.x += (m_absoluteRect.size.x - totalWidth) * 0.5F;
			}

			for (Element* child : elements)
			{
				child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
				child->m_absoluteRect.size.y *= m_scaleHeight ? heightRatio : 1.0F;
				t_currentPosition.x += child->m_absoluteRect.size.x;
			}
		}

	public:
		AlignStyleHorizontal
							m_justify = AlignStyleHorizontal::kLeft;
		bool				m_scaleHeight = true;
		bool				m_heightLocked = false; // Can the height of this element change?
	};

	class StretchGridRows : public dusk::LayoutElement
	{
	public:
		ENGCOM_API explicit		StretchGridRows ( void ) 
			: dusk::LayoutElement()
			{}

		//	LayoutChildren() : Called every frame by the UI system.
		void					LayoutChildren ( std::vector<Element*>& elements ) override
		{
			// Grab the parent's rect for the full initial size, first.
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

			// Loop through all elements and generate how we will layout first.
			std::vector<bool> elementCanStretch;
			elementCanStretch.reserve(elements.size());
			float totalHeight = 0.0F;
			float totalHeightNoStretch = 0.0F;
			for (Element* child : elements)
			{
				elementCanStretch.push_back(true);
				if (child->GetElementType() == ElementType::kLayout)
				{
					totalHeight += child->m_localRect.size.y;

					// If it's a stretchgridrow, then check if its height is locked and should be skipped from the scaling.
					StretchGridRow* childAsRow = child->asSafeAndSlow<StretchGridRow>();
					if (childAsRow != NULL)
					{
						if (childAsRow->m_heightLocked)
						{
							totalHeightNoStretch += child->m_localRect.size.y;
							elementCanStretch.back() = false;
						}
					}
				}
				else
				{
					totalHeight += child->m_localRect.size.y;
				}
			}

			// Now lay out all the elements vertically.
			Vector2f t_currentPosition = m_absoluteRect.pos;
			uint currentChildIndex = 0;
			float verticalStretchRatio = (m_absoluteRect.size.y - totalHeightNoStretch) / (totalHeight - totalHeightNoStretch);
			for (Element* child : elements)
			{
				if (elementCanStretch[currentChildIndex++])
				{
					// scale up the stretchy bits by taking the ratio of the 
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
					child->m_absoluteRect.size.y *= verticalStretchRatio;
				}
				else
				{
					child->m_absoluteRect = Rect(t_currentPosition, child->m_localRect.size);
				}
				t_currentPosition.y += child->m_absoluteRect.size.y;
			}
		}

	};

}}

#endif//ENGINE_COMMON_DUSK_UI_LAYOUT_GRID_H_