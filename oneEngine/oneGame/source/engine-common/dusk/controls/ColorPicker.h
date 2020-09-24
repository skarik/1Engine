#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_COLOR_PICKER_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_COLOR_PICKER_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	class ColorPicker : public dusk::Element
	{
	public:
		ENGCOM_API explicit		ColorPicker()
			: dusk::Element()
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		bool				m_useAlpha = false;

		Color				m_value = Color(1.0F, 1.0F, 1.0F, 1.0F);
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_COLOR_PICKER_H_