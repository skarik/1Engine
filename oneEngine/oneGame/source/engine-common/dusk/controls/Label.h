#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_LABEL_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_LABEL_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	enum LabelStyle
	{
		kLabelStyle_Normal,
		kLabelStyle_Heading1,
		kLabelStyle_Heading2,
		kLabelStyle_Heading3,
	};

	class Label : public dusk::Element
	{
	public:
		ENGCOM_API explicit		Label ( void ) 
			: dusk::Element()
		{
			m_canFocus = false;
		}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

	public:
		LabelStyle			m_style = kLabelStyle_Normal;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_LABEL_H_