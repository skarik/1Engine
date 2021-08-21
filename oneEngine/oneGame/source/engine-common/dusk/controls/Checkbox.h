#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_CHECKBOX_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_CHECKBOX_H_

#include "engine-common/dusk/controls/Button.h"

namespace dusk {
namespace elements {

	class Checkbox : public dusk::elements::Button
	{
	public:
		ENGCOM_API explicit		Checkbox()
			: dusk::elements::Button()
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		//	SetOnValueChange ( callback ) : Set default callback to call on value change
		ENGCOM_API void			SetOnValueChange ( std::function<void(bool)>&& fn )
			{ m_onValueChange = fn; }

	public:
		bool				m_value = false;

		std::function<void(bool)>
							m_onValueChange = nullptr;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_CHECKBOX_H_