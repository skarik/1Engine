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
	class MetaElement;
	class LayoutElement;
	class DialogElement;

	struct ElementCreationDescription
	{
		Element*	parent;
		Rect		localRect;

		ElementCreationDescription()
			: parent(NULL), localRect()
			{}

		ElementCreationDescription(Element* in_parent, Rect in_rect)
			: parent(in_parent), localRect(in_rect)
			{}
	};

	struct LayoutCreationDescription : public ElementCreationDescription
	{
		LayoutCreationDescription(Element* in_parent)
			: ElementCreationDescription(in_parent, Rect())
			{}

		LayoutCreationDescription(Element* in_parent, Vector2f in_size)
			: ElementCreationDescription(in_parent, Rect(Vector2f(), in_size))
			{}
	};

	struct DialogCreationDescription : public ElementCreationDescription
	{
		DialogCreationDescription()
			: ElementCreationDescription(NULL, Rect())
			{}
	};

	enum ElementHandleConstants : uint32_t
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
		ElementType*			Add ( const ElementCreationDescription& desc )
		{
			static_assert(ElementType::IsDialogElement() == false, "Dialogs cannot be added normally.");
			ElementType* item = new ElementType();
			AddInitialize(item, desc);
			m_elements.push_back(item); // New element, tree needs regen.
			m_treeNeedsGeneration = true; 
			ARCORE_ASSERT(AddInitializeCheckValid(item));
			item->PostCreate();
			return item;
		}

		//	CreateDialog()
		template <class ElementType>
		ElementType*			AddDialog ( const ElementCreationDescription& desc )
		{
			static_assert(ElementType::IsDialogElement(), "Controls cannot be added via dialog creation.");
			ARCORE_ASSERT(desc.parent == NULL);
			ElementType* item = new ElementType();
			AddInitialize(item, desc);
			m_elements.push_back(item); // New element, tree needs regen.
			m_treeNeedsGeneration = true; 
			ARCORE_ASSERT(AddInitializeCheckValid(item));
			item->PostCreate();
			return item;
		}

		//	EnterDialogue(element) : Enters the element as a dialogue.
		ENGCOM_API void			EnterDialogue ( Element* element );

		//	ExitDialogue(element) : Exit dialogue mode of the system.
		ENGCOM_API void			ExitDialogue ( Element* element );

		//	IsMouseInside() : Checks if mouse cursor is currently inside any active element
		ENGCOM_API bool			IsMouseInside ( void );

		//	DestroyElement(index) : Removes the element with the given index/handle.
		// Should only be used in the Element destructor.
		void					RemoveElement ( const size_t handle );

	private:

		//	AddInitialize(element, desc) : Initializes the element with the given description.
		// Used after an element is instantiated, applies the given parameters.
		ENGCOM_API Element*		AddInitialize ( Element* element, const ElementCreationDescription& desc );

		//	AddInitializeCheckValid(element) : Checks the given element has the correct index.
		// To be used as part of an assertion check.
		ENGCOM_API bool			AddInitializeCheckValid ( Element* element );

		//	DestroyElement(index) : Destroys the element with the given index/handle.
		// Used internally to remove elements.
		void					DestroyElement ( const size_t handle, const bool also_delete = true );

		//	ClearElementTree() : Destroys the entire element tree.
		void					ClearElementTree ( void );


		void					GenerateElementTree ( void );
		void					UpdateElementPositions ( void );
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

		std::vector<std::vector<bool>>
							m_dialogueActivationStack;

		// List of rects that must be updated & re-rendered.
		std::vector<Rect>	m_forcedUpdateAreas;

		// TODO: track those with visibility states that were toggled

		struct ElementNode
		{
			uint32_t			index = 0;
			std::vector<ElementNode*>
								children = std::vector<ElementNode*>(0);
		};
		ElementNode*		m_elementTreeBase;
		bool				m_treeNeedsGeneration = false;

	private:
		friend UIRenderer; // Give UI renderer dangerous access to sidestep creating accessors for only one class.
		UIRenderer*			m_renderer;
	};
}

#endif//ENGINE_COMMON_DUSK_UI_H_