

#ifndef _GB_BOOL_H_
#define _GB_BOOL_H_

#include "core/types/types.h"

class CGameBehavior;

class gbbool
{
public:
	ENGINE_API	gbbool ( void ) : value ( true ) {}
	ENGINE_API	gbbool ( bool val ) : value ( val ) {}
	ENGINE_API	gbbool& operator= ( bool const &rhs );
	ENGINE_API	operator const bool() const {return value;} 

	ENGINE_API	void setTarget ( CGameBehavior* newTarget ) { target=newTarget; }
private:
	bool value;
	CGameBehavior* target;
};

#include "CGameBehavior.h"

#endif