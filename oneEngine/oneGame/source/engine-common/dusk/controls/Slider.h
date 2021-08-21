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
		ENGCOM_API void			Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		ENGCOM_API void			Render ( UIRendererContext* uir ) override;

		//	SetOnValueChange ( callback ) : Set default callback to call on value change
		ENGCOM_API void			SetOnValueChange ( std::function<void(NumberType)>&& fn )
			{ m_onValueChange = fn; }

	public:
		NumberType			m_range_min = (NumberType)0.0;
		NumberType			m_range_max = (NumberType)1.0;
		bool				m_snap = false;
		NumberType			m_snap_divisor = (NumberType)0.1;

		NumberType			m_value = (NumberType)0.5;

		std::function<void(NumberType)>
							m_onValueChange = nullptr;
	};

}}

//#include "Slider.hpp"

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_SLIDER_H_