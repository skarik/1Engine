#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_H_

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "engine-common/dusk/Handle.h"
#include <string>
#include <vector>

namespace dusk
{
	class UserInterface;
	class UIRenderer;
	class UIRendererContext;
	class UIRendererElementColors;

	struct UIStepInfo
	{
		Vector2f			mouse_position;
	};

	enum class ElementType : uint8_t
	{
		// Element is a normal active control
		kControl	= 0,
		// Element only exists to contain data.
		// Cannot be selected or focused.
		// Is updated normally, but is skipped in rendering.
		kMeta		= 1,
		// Element is used to layout its children.
		// Cannot be selected or focused.
		// Is updated normally, but is skipped in rendering. Has special callback for updating children elements.
		kLayout		= 2,
	};

	class Element
	{
	public:
		// Constructor for defaul val
		explicit				Element ( const ElementType inElementType = ElementType::kControl )
			: m_elementType(inElementType)
			{}
		virtual					~Element ( void )
			{}

		//	Update() : Called every frame by the UI system.
		// Default behavior is to update value of m_isMouseIn.
		virtual void			Update ( const UIStepInfo* stepinfo );
		//	Render() : Renders the element.
		// This does not actually directly render anything, but create meshes through the interface provided in UIRenderer.
		// Default behavior is to not render anything.
		virtual void			Render ( UIRendererContext* uir )
			{}

		//	as<Element>() : Shorthand element typecast
		template <typename T>
		T*						as (void)
			{ return static_cast<T*>(this); }

		//	IsDialogType() : Is this a dialog? Used for type-safe instantiation of dialogs.
		static constexpr bool	IsDialogElement ( void )
			{ return false; }

	public:
		// Set to true when needs deletion by system
		bool				m_destructionRequested = false;

		// Local rect for positioning
		Rect				m_localRect;
		// Ignore position set up by automatic layout elements
		bool				m_ignoreAutoLayout = false;
		// Ignore positoin set up by any layout
		bool				m_overrideLayout = false;
		// Reference to parent.
		dusk::Element*		m_parent = NULL;
		// String for contents, often a label.
		std::string			m_contents = "";
		// String for tooltip, shown when mouse hovers over the element rect.
		std::string			m_tooltip = "";
		// Is currently visible
		bool				m_visible = true;
		// Can be focused?
		bool				m_canFocus = true;

		// Mouse is in element...?
		bool				m_isMouseIn = false;
		// Element has focus
		bool				m_isFocused = false;
		// Element is active and not locked
		bool				m_isEnabled = true;
		// Element was activated this frame (ie clicked, used)?
		bool				m_isActivated = false;
		// Element was drawn last frame
		bool				m_wasDrawn = false;

		// Result actual rect
		Rect				m_absoluteRect;

	protected:
		friend UserInterface;
		friend UIRenderer;
		friend UIRendererContext;
		friend UIRendererElementColors;

		// The GUI system that this element is associated with
		UserInterface*		m_interface;

	private:

		Rect				m_lastRenderedRect;
		uint32_t			m_index;
		Vector4f			m_rendererData;

		// Element type. Not accessible after construction to child classes.
		ElementType			m_elementType;
	};

	class DialogElement : public Element
	{
	public:
		// Constructor for defaul val
		explicit				DialogElement ( void )
			: Element(ElementType::kControl)
			{}

		//	Show() : Shows the dialog.
		// Must be called in order to bring this dialog to the forefront
		virtual void			Show ( void )
			{}

		//	Hide() : Hides the dialog.
		virtual void			Hide ( void )
			{}

		//	IsDialogType() : Is this a dialog? Used for type-safe instantiation of dialogs.
		static constexpr bool	IsDialogElement ( void )
			{ return true; }

	public:
		// Is this dialog currently open?
		bool				m_isOpen = false;
	};

	class MetaElement : public Element
	{
	public:
		// Constructor for defaul val
		explicit				MetaElement ( void )
			: Element(ElementType::kMeta)
			{}
	};

	class LayoutElement : public Element
	{
	public:
		// Constructor for defaul val
		explicit				LayoutElement ( void )
			: Element(ElementType::kLayout)
			{}

		//	ResizeChildren() : Called every frame by the UI system.
		// Requires list of children of the element that need to be laid out.
		// Resizes are executed from children-to-parent order.
		virtual void			ResizeChildren ( std::vector<Element*>& elements )
			{}

		//	LayoutChildren() : Called every frame by the UI system.
		// Requires list of children of the element that need to be laid out.
		// Layouts are executed from parent-to-children order. Thus, some width-dependant layouts may take several frames to update correctly.
		virtual void			LayoutChildren ( std::vector<Element*>& elements )
			{}
	};

};

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_H_