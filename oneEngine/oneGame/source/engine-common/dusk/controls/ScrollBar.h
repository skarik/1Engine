#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_SCROLL_BAR_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_SCROLL_BAR_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	class ScrollBar : public dusk::Element
	{
	public:
		ENGCOM_API explicit		ScrollBar();

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		//	SetScroll(value) : Sets the current scroll position.
		// Input must be between 0 and m_scrollSize. It will be clamped otherwise.
		void					SetScroll ( float scroll );
		//	GetScroll() : Returns the current scroll position.
		float					GetScroll ( void )
			{ return m_scrollPosition; }

		//	GetPreferredWidth() : Returns optimal width for viewing
		static constexpr float	GetPreferredWidth ( void ) { return 20.0F; }

	public:
		
		enum class Orientation
		{
			kHorizontal,
			kVertical,
		};
		Orientation			m_orientation = Orientation::kVertical;

		enum class ButtonPosition
		{
			kBothAtStart,
			kSplit,
			kBothAtEnd,
		};
		ButtonPosition		m_buttonPosition = ButtonPosition::kSplit;

		float				m_scrollSize = 100.0F;
		float				m_viewSize = 10.0F;
		float				m_buttonLengthInPixels = GetPreferredWidth();

	private:
		float				m_scrollPosition = 0.0F;

		struct SubelementState
		{
			bool				hovered = false;
			bool				activated = false;
			Rect				rect;
		};
		SubelementState		m_substateButtonUp;
		SubelementState		m_substateButtonDown;
		SubelementState		m_substateBar;
		SubelementState		m_substateScroller;

		bool				m_draggingBar = false;
		Vector2f			m_referenceMouseDragScroll;
		Vector2f			m_referenceMouseDragPosition;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_SCROLL_BAR_H_