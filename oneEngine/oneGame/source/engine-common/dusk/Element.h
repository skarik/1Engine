#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_H_

#include "core/math/Rect.h"
#include "core/math/Color.h"
#include "engine-common/dusk/Handle.h"
#include <string>

namespace Dusk
{
	class UserInterface;

	class Element
	{
	public:
		// Constructor for defaul val
		explicit Element ( void ) :
			m_destructionRequested( false ),
			m_parent(NULL), m_parentHandle(-1, NULL),
			m_contents(""), m_tooltip(""),
			m_visible(true), m_canFocus(true),
			m_isMouseIn(false), m_isFocused(false), m_wasDrawn(false)
			{}
		virtual ~Element ( void )
			{}

		//	Update() : Called every frame by the UI system.
		// Default behavior is to update value of m_isMouseIn.
		virtual void Update ( void );
		//	Render() : Renders the element.
		// Default behavior is to not render anything.
		virtual void Render ( void )
			{}

	public:
		// Set to true when needs deletion by system
		bool				m_destructionRequested;

		// Local rect for positioning
		Rect				m_localRect;
		// Reference to parent.
		Dusk::Element*		m_parent;
		Dusk::Handle		m_parentHandle;
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
		// Element was drawn last frame
		bool				m_wasDrawn;
		// Element is active and not locked
		bool				m_isEnabled;

	protected:
		friend UserInterface;
		
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

	};
};

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_H_