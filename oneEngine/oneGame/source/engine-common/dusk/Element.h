#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_H_

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "engine-common/dusk/Handle.h"
#include <string>

namespace dusk
{
	class UserInterface;
	class UIRenderer;
	class UIRendererContext;

	struct UIStepInfo
	{
		Vector2f	mouse_position;
	};

	class Element
	{
	public:
		// Constructor for defaul val
		explicit				Element ( void ) :
			m_destructionRequested( false ),
			m_parent(NULL),
			m_contents(""), m_tooltip(""),
			m_visible(true), m_canFocus(true),
			m_isMouseIn(false), m_isFocused(false), m_isEnabled(true), m_isActivated(false),
			m_wasDrawn(false)
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

	public:
		// Set to true when needs deletion by system
		bool				m_destructionRequested;

		// Local rect for positioning
		Rect				m_localRect;
		// Reference to parent.
		dusk::Element*		m_parent;
		// String for contents, often a label.
		std::string			m_contents;
		// String for tooltip, shown when mouse hovers over the element rect.
		std::string			m_tooltip;
		// Is currently visible
		bool				m_visible;
		// Can be focused?
		bool				m_canFocus;

		// Mouse is in element...?
		bool				m_isMouseIn;
		// Element has focus
		bool				m_isFocused;
		// Element is active and not locked
		bool				m_isEnabled;
		// Element was activated this frame (ie clicked, used)?
		bool				m_isActivated;
		// Element was drawn last frame
		bool				m_wasDrawn;

	protected:
		friend UserInterface;
		friend UIRenderer;
		friend UIRendererContext;
		
		// Result actual rect
		Rect				m_absoluteRect;

		// The GUI system that this element is associated with
		UserInterface*		m_interface;

		/*setDown, setHover, setDefault
		drawRect( Rect )
		drawRectWire( Rect )
		drawText( x,y,string )*/

		//=========================================//
		// GUI Rendering interface
		//=========================================//

		//void setDrawDown ( void );
		//void setDrawHover ( void );
		//void setDrawDefault ( void );

		//// These two cannot mix
		//void setSubdrawSelection ( void );
		//void setSubdrawDarkSelection ( void );
		//// These two cannot mix
		//void setSubdrawTransparent ( void );
		//void setSubdrawOpaque ( void );
		//// These two cannot mix
		//void setSubdrawNotice ( void );
		//void setSubdrawError ( void );
		//// Special states
		//void setSubdrawPulse ( void );
		//void setSubdrawOverrideColor ( const Color& color );
		//// Reset states
		//void setSubdrawDefault ( void );

		//void drawRect ( const Rect& rect );
		//void drawRectWire ( const Rect& rect );
		//void drawLine ( const Real x1, const Real y1, const Real x2, const Real y2 );

		//void drawText ( const Real x, const Real y, const char* str );
		//void drawTextWidth ( const Real x, const Real y, const Real w, const char* str );
		//void drawTextCentered ( const Real x, const Real y, const char* str );

		//rrTextBuilder2D* getMeshBuilder ( void );

	private:

		//Rect last_visible_rect;
		Rect				m_lastRenderedRect;
		//bool				m_lastRenderedFrame;

		uint32_t			m_index;

		Vector4f			m_rendererData;
	};
};

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_H_