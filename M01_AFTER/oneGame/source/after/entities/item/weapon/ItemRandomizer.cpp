
#include "ItemRandomizer.h"

#include <stdlib.h>
#include <string.h>

bool WeaponItem::IsPart ( const int type, const char* name )
{
	switch ( type ) {
	case 0:
		if ( strcmp( name, "part0" ) == 0 ) return true;
		if ( strcmp( name, "blade" ) == 0 ) return true;
		if ( strcmp( name, "head" ) == 0 ) return true;
		break;
	case 1:
		if ( strcmp( name, "part1" ) == 0 ) return true;
		if ( strcmp( name, "hilt" ) == 0 ) return true;
		if ( strcmp( name, "handle" ) == 0 ) return true;
		break;
	case 2:
		if ( strcmp( name, "part2" ) == 0 ) return true;
		break;
	case 3:
		if ( strcmp( name, "part3" ) == 0 ) return true;
		break;
	}

	return false;
}
void WeaponItem::ModifyNumber ( ftype& inOutParamter, const char* input )
{
	ftype baseModifier = (Real)atof( input );
	if ( strstr(input,"%") != NULL ) {
		inOutParamter += (inOutParamter*baseModifier)/100.0f;
	}
	else {
		inOutParamter += baseModifier;
	}
}