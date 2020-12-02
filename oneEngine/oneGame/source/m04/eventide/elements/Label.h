#ifndef UI_EVENTIDE_ELEMENTS_LABEL_H_
#define UI_EVENTIDE_ELEMENTS_LABEL_H_

#include "m04/eventide/Element.h"

namespace ui {
namespace eventide {
namespace elements {

	class Label : public ui::eventide::Element
	{
	public:
		EVENTIDE_API			Label ( ui::eventide::UserInterface* ui = NULL );
		EVENTIDE_API			~Label ( void );

		virtual void			BuildMesh ( void ) override;

		// Label displayed within the element.
		std::string				m_contents;

	private:
		ui::eventide::Texture	m_fontTexture;
	};

}}}

#endif//UI_EVENTIDE_ELEMENTS_LABEL_H_