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

namespace dusk
{
	class UIRenderer;
	class Element;

	struct ElementCreationDescription
	{
		Element*	parent;
		Rect		localRect;

		ElementCreationDescription()
			: parent(NULL), localRect()
			{}
	};

	enum ElementHandleConstants
	{
		kElementHandleInvalid = 0xFFFFFFFF,
	};

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
		ENGCOM_API const std::vector<Element*>&
								ElementList ( void )
			{ return m_elements; }

		//	Add() : Adds a new element of the given type.
		template <class ElementType>
		ENGCOM_API ElementType*	Add ( const ElementCreationDescription& desc )
		{
			ElementType* item = new ElementType();
			AddInitialize(item, desc);
			m_elements.push_back(item);
			ARCORE_ASSERT(AddInitializeCheckValid(item));
			return item;
		}

		//	EnterDialogue(element) : Enters the element as a dialogue.
		ENGCOM_API void			EnterDialogue ( Element* element );

		//	ExitDialogue(element) : Exit dialogue mode of the system.
		ENGCOM_API void			ExitDialogue ( Element* element );

	private:

		//	AddInitialize(element, desc) : Initializes the element with the given description.
		// Used after an element is instantiated, applies the given parameters.
		Element*				AddInitialize ( Element* element, const ElementCreationDescription& desc );

		//	AddInitializeCheckValid(element) : Checks the given element has the correct index.
		// To be used as part of an assertion check.
		bool					AddInitializeCheckValid ( Element* element );

		//	DestroyElement() : Destroys the element with the given index/handle.
		// Used internally to remove elements.
		void					DestroyElement ( const size_t handle );


		void					UpdateMouseOver ( void );
		void					UpdateFocus ( void );
		void					UpdateElements ( void );

	public:

		Vector2f			m_cursor; // accessible publicly? 
		//Vector2f( CInput::MouseX(), CInput::MouseY() );
		// offsets: set the element's true position?

	private:
		// List of elements
		std::vector<Element*>
							m_elements;
		uint32_t			m_currentElement;
		uint32_t			m_currentDialogue;
		uint32_t			m_currentMouseover;
		uint32_t			m_currentFocus;

		// List of rects that must be updated & re-rendered.
		std::vector<Rect>	m_forcedUpdateAreas;

		//std::vector<Vector2f> offsetList;

		std::vector<Vector2f>
							m_updateOffsets;


	private:
		friend UIRenderer; // Give UI renderer dangerous access to sidestep creating accessors for only one class.
		UIRenderer*			m_renderer;
	};
}

#endif//ENGINE_COMMON_DUSK_UI_H_