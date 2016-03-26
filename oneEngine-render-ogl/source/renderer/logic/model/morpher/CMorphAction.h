
#ifndef _C_MORPH_ACTION_H_
#define _C_MORPH_ACTION_H_

#include "core/types/types.h"
#include <string>
using std::string;

class CMorphAction
{
public:
	// Constructor
	CMorphAction ( const char* name ) {
		actionName = name;
		weight = 0;
		index = 0;

		auto_blend = false;
		target_weight = 0;
	}
	CMorphAction ( void ) {
		actionName = "unnamed";
		weight = 0;
		index = 0;

		auto_blend = false;
		target_weight = 0;
	}

	Real	weight;
	Real	target_weight;
	short	index;
	char	auto_blend;

	// == Getters ==
	const arstring128& GetName ( void ) const
	{
		return actionName;
	};
private:
	arstring128	actionName;
};

#endif//_C_MORPH_ACTION_H_