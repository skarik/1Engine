
#ifndef _C_TERRA_FOLIAGE_FACTORY_H_
#define _C_TERRA_FOLIAGE_FACTORY_H_

class CFoliage;

//Where/what are these?
#include "core/common.h"
#include "core/types/types.h"

#include "core-ext/transform/CTransform.h"

#include <typeinfo>
#include <string>

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif

namespace TerraFoliage
{
	unsigned short GetFoliageType ( CFoliage* pFoliage );
	unsigned short GetFoliageType ( const std::string& stName );

	CFoliage* CreateFoliage ( unsigned short iType, Vector3d const& vInPos, const char* iInData );
}

#endif//_C_TERRA_FOLIAGE_FACTORY_H_;