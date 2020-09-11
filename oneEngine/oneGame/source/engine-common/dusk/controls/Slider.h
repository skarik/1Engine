#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_SLIDER_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_SLIDER_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	template <typename NumberType>
	class Slider : public dusk::Element
	{
	public:
		ENGCOM_API explicit		Slider ( void ) 
			: dusk::Element()
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		NumberType			m_range_min = 0.0F;
		NumberType			m_range_max = 1.0F;
		bool				m_snap = false;
		NumberType			m_snap_divisor = 0.1F;

		NumberType			m_value = 0.5F;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_SLIDER_H_