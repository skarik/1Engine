#ifndef EVENTIDE_TEST_CUBIC_LABEL_H_
#define EVENTIDE_TEST_CUBIC_LABEL_H_

#include "m04/eventide/Element.h"

class ETCubicLabel : public ui::eventide::Element
{
	//
public:
							ETCubicLabel ( ui::eventide::UserInterface* ui = NULL );
							~ETCubicLabel ( void );

	virtual void			BuildMesh ( void ) override;

	virtual void			OnEventMouse ( const EventMouse& mouse_event ) override;

private:
	ui::eventide::Texture	m_fontTexture;

};

#endif//EVENTIDE_TEST_CUBIC_LABEL_H_