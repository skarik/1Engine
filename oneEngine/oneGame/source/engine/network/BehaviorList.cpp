
#include "BehaviorList.h"

ARSINGLETON_CPP_DEF(engine::BehaviorList);

valuetype_t Network::ValueTypeFromName ( const char* n_typename )
{
	if ( strcmp(n_typename, "int32_t") == 0 )
	{
		return VALUE_INT32;
	}
	if ( strcmp(n_typename, "float") == 0 )
	{
		return VALUE_FLOAT;
	}
	if ( strcmp(n_typename, "Vector2f") == 0 )
	{
		return VALUE_FLOAT2;
	}
	if ( strcmp(n_typename, "Vector3d") == 0 )
	{
		return VALUE_FLOAT3;
	}
	if ( strcmp(n_typename, "Vector4d") == 0 )
	{
		return VALUE_FLOAT4;
	}
	if ( strcmp(n_typename, "Color") == 0 )
	{
		return VALUE_FLOAT4;
	}
	if ( strcmp(n_typename, "Quaternion") == 0 )
	{
		return VALUE_FLOAT4;
	}

	printf(
		"\n\n====================================\n"
		"ERROR: COULD NOT FIND MATCHING TYPE FOR \"%s\" IN SERIALIZER (Network::ValueTypeFromName())\n"
		"====================================\n\n", n_typename);
	return VALUE_INVALID;
}