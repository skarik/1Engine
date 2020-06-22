#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_PANEL_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_PANEL_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	class Panel : public dusk::Element
	{
	public:
		ENGCOM_API explicit		Panel (void)
			: dusk::Element()
		{
			m_canFocus = false;
		}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		bool				m_displayLabel = true;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_PANEL_H_