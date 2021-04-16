#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_FIELD_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_FIELD_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	class TextField : public dusk::Element
	{
	public:
		ENGCOM_API explicit		TextField()
			: dusk::Element()
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		Real				m_drawHeight = 0.0F;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_FIELD_H_