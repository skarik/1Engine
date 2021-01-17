#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_SPACER_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_SPACER_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	enum class SizePreference
	{
		kSmaller,
		kNone,
		kLarger,
	};

	class Spacer : public dusk::Element
	{
	public:
		ENGCOM_API explicit		Spacer ( void ) 
			: dusk::Element()
		{
			m_canFocus = false;
		}

		//	PostCreate() : Called after the element has been created and added to the UI.
		void					PostCreate ( void ) override;
		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override
			{}

	public:
		Vector2f			m_minSize = Vector2f(-1.0F, -1.0F);
		Vector2f			m_maxSize = Vector2f(-1.0F, -1.0F);
		SizePreference		m_widthPreference = SizePreference::kNone;
		SizePreference		m_heightPreference = SizePreference::kNone;

	private:
		Vector2f			m_parentReferenceSize = Vector2f(-1.0F, -1.0F);
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_SPACER_H_