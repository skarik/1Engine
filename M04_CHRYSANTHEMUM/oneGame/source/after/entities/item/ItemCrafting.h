
#ifndef _ITEM_CRAFTING_H_
#define _ITEM_CRAFTING_H_

#include <vector>
#include <string>
using std::string;

#include "core/containers/arstring.h"

#include "CWeaponItemTypes.h"

struct tItemRecipe
{
public:

	// Ingredient struct
	struct item_entry
	{
		// Item ID
		short id;
		// Item SubID (For terrabloks and other stuff like that)
		short subid; 
		// Item required stack size
		short stack;
		// Userdata, normally for name of Lua item
		string userdata;

		// constructor
		item_entry ( void ) : id(0), subid(0), stack(1) {}
	};

	// Recipe name
	string name;
	// Recipe width
	int width;
	// Recipe height
	int height;
	// Required forge tier
	int forge;
	// Required player level
	int level;
	// Internal data array
	std::vector<item_entry> slot;

	// Result item id
	/*short id;
	// Result item subid (For terrabloks and other stuff like that)
	short subid; 
	// Result item stack
	short stack;*/
	item_entry result;

	// If recipe has been unlocked
	bool unlocked;

	// Easy data dereference
	item_entry& operator () ( const unsigned char x, const unsigned char y ) {
		return slot[x+y*width];
	}
	const item_entry& operator () ( const unsigned char x, const unsigned char y ) const {
		return slot[x+y*width];
	}
	// quick set size
	void setSize ( const int new_w, const int new_h ) {
		width = new_w;
		height = new_h;
		slot.resize( new_w*new_h );
	}

	// constructor
	tItemRecipe ( void ) : width(0), height(0), unlocked(true) {}
};


#endif