
// CDuskGUIPropertyview
//  A propertyview control is similar to a list view, in that it's elements are laid out
//  vertically, and going beyond the range of the control causes a scrollbar.
//  However, each element has a type and a corresponding field to set it with.
//  This field mostly has the same behavior as a floatfield, textfield, or other selector.
//  
//  

#ifndef _C_DUSK_GUI_PROPERTYVIEW_H_
#define _C_DUSK_GUI_PROPERTYVIEW_H_

#include "CDuskGUIPanel.h"
/*
class base_propertyview_entry
{
	bool dirty;
};

template <typename type>
class PropertyViewEntry : public base_propertyview_entry
{
	type previous;
	type* value;
};
*/
class CDuskGUIPropertyview : public CDuskGUIPanel
{
public:
	CDuskGUIPropertyview ( void ) : selection(-1), CDuskGUIPanel(21), field_height(0.04f), name_width(0.3f), scrollbar_width(0.03f), scroll_offset(-0.2f) { ; };
	~CDuskGUIPropertyview ( void );

	// Update and render routines
	void Update ( void );
	void Render ( void );

	// Modifying the list
	template <typename type>
	void AddToList ( type* value );

	// Element for list
	struct ListElement_t {
		string		str;
		uint		type;
		uint64_t datas[4]; // Storage area for values, 128 bits each
		void*	previous;
		void*	value;
		void*	target;
		bool	dirty;
		CDuskGUI::Handle	element;

		ListElement_t ( void ) {
			datas[0]	= 0;
			datas[1]	= 0;
			datas[2]	= 0;
			datas[3]	= 0;
			previous	= datas+0;
			value		= datas+2;
			target		= NULL;
			dirty		= true;
			type		= 0;
			str			= "";
			element		= -1;
		};

		ListElement_t(const ListElement_t &source)
		{
			datas[0]	= source.datas[0];
			datas[1]	= source.datas[1];
			datas[2]	= source.datas[2];
			datas[3]	= source.datas[3];
			previous	= datas+0;
			value		= datas+2;
			target		= source.target;
			dirty		= source.dirty;
			type		= source.type;
			str			= source.str;
			element		= source.element;
		}

		ListElement_t& operator= (const ListElement_t &source)
		{
			datas[0]	= source.datas[0];
			datas[1]	= source.datas[1];
			datas[2]	= source.datas[2];
			datas[3]	= source.datas[3];
			previous	= datas+0;
			value		= datas+2;
			target		= source.target;
			dirty		= source.dirty;
			type		= source.type;
			str			= source.str;
			element		= source.element;
			return *this;
		};
	};

	template <typename type>
	type Get ( void* ptr ) {
		return *((type*)(ptr));
	}
	template <typename type>
	void Set ( void* ptr, type val ) {
		*((type*)(ptr)) = val;
	}

public:
	std::vector<ListElement_t> propertyList;
	int  selection;
	int  currentmouseover;
	ftype field_height;
	ftype name_width;
	ftype scrollbar_width;
	
	ftype scroll_offset;
	ftype max_scroll_offset;
};

#endif//_C_DUSK_GUI_PROPERTYVIEW_H_