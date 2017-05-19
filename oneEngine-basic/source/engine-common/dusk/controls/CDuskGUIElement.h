
#ifndef _C_DUSK_GUI_ELEMENT_
#define _C_DUSK_GUI_ELEMENT_

// Includes
#include "core/math/Rect.h"
#include "core/math/Color.h"
#include <string>
using std::string;

#include "../CDuskGUIHandle.h"

// Class Prototype
class CDuskGUI;

// Class Definition
class CDuskGUIElement
{
public:
	// Handle type
	//typedef int Handle;
	typedef Dusk::Handle Handle;
	
public:
	// Constructor for defaul val
	explicit CDuskGUIElement ( const int ntype ) :
		delete_me( false ),
		parent(-1), visible( true ), hasFocus( false ), canHaveFocus(true),
		tooltip(""), drawn(false),
		m_type(ntype)
	{
		;
	}
	virtual ~CDuskGUIElement ( void )
	{
		;
	}

	// Overridable update
	virtual void Update ( void );
	virtual void Render ( void ) =0;

public:
	// Set to true when needs deletion by system
	bool delete_me;

	// Rect for positioning
	Rect rect;
	// Reference to parent
	Handle parent;
	// String for label
	string label;
	// String for tooltip
	string tooltip;
	// Is currently visible
	bool visible;

	// Mouse is in element...?
	bool	mouseIn;
	// Element has focus
	bool	hasFocus;
	// Element can have focus
	bool	canHaveFocus;
	// Element was drawn last frame
	bool	drawn;

	// Element typeid
	const int m_type;

public:
	static Vector2d cursor_pos;
	static CDuskGUI* activeGUI;

protected:
	/*setDown, setHover, setDefault
	drawRect( Rect )
	drawRectWire( Rect )
	drawText( x,y,string )*/
	
	
	//=========================================//
	// GUI Rendering interface
	//=========================================//

	void setDrawDown ( void );
	void setDrawHover ( void );
	void setDrawDefault ( void );

	// These two cannot mix
	void setSubdrawSelection ( void );
	void setSubdrawDarkSelection ( void );
	// These two cannot mix
	void setSubdrawTransparent ( void );
	void setSubdrawOpaque ( void );
	// These two cannot mix
	void setSubdrawNotice ( void );
	void setSubdrawError ( void );
	// Special states
	void setSubdrawPulse ( void );
	void setSubdrawOverrideColor ( const Color& color );
	// Reset states
	void setSubdrawDefault ( void );

	void drawRect ( const Rect& rect );
	void drawRectWire ( const Rect& rect );
	void drawLine ( const Real x1, const Real y1, const Real x2, const Real y2 );
	
	void drawText ( const Real x, const Real y, const char* str );
	void drawTextWidth ( const Real x, const Real y, const Real w, const char* str );
	void drawTextCentered ( const Real x, const Real y, const char* str );

private:
	friend CDuskGUI;

	Rect last_visible_rect;

};

#endif