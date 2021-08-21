#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_FLOAT_FIELD_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_FLOAT_FIELD_H_

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/controls/TextField.h"

namespace dusk {
namespace elements {

	class FloatField : public dusk::elements::TextField
	{
	public:
		ENGCOM_API explicit		FloatField()
			: dusk::elements::TextField()
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;

		//	GetValue() : Returns current value.
		ENGCOM_API float		GetValue ( void );
	public:
		Real				m_drawHeight = 0.0F;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_FLOAT_FIELD_H_