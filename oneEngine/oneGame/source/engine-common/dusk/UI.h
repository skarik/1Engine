//===============================================================================================//
//
//		Dusk UI : class UserInterface
//
// UserInterface is the main Dusk controller. To use Dusk UI, instantiate a UserInterface object,
// then configure as needed.
//
//===============================================================================================//
#ifndef ENGINE_COMMON_DUSK_UI_H_
#define ENGINE_COMMON_DUSK_UI_H_

#include "engine/behavior/CGameBehavior.h"
#include "engine-common/dusk/Handle.h"

namespace Dusk
{
	class UIRenderer;
	class Element;

	class UserInterface : public CGameBehavior
	{
		ClassName( "DuskGUI" );

	public:
		ENGCOM_API explicit		UserInterface ( void );
		ENGCOM_API				~UserInterface ( void );

		//	Update() : Called by engine.
		// Performs per-frame behavior updates of the UI, generates layouts, and sets up information
		// needed for display.
		void					Update ( void ) override;

		//	ElementList() : Get the current list of elements.
		// Returns list of all the active elements.
		ENGCOM_API std::vector<Element*>& const
								ElementList ( void )
			{ return m_elements; }

	private:
		// List of elements
		std::vector<Element*>
							m_elements;
		Handle				m_currentElement;
		Handle				m_currentDialogue;
		Handle				m_currentMouseover;
		Handle				m_currentFocus;

		// List of rects that must be updated & re-rendered.
		std::vector<Rect>	m_forceUpdateRects;

	private:
		UIRenderer*			m_renderer;
	};
}

#endif//ENGINE_COMMON_DUSK_UI_H_