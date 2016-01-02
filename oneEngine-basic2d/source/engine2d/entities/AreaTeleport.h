
#ifndef _C_AREA_2D_TELEPORT_H_
#define _C_AREA_2D_TELEPORT_H_

#include "core/math/Color.h"

#include "Area2DBase.h"

namespace Engine2D
{
	enum class eTeleportType
	{
		EdgeOrNormal,
		Boring,
		Glitch
	};

	class AreaTeleport : public Area2DBase
	{
		ClassName("AreaTeleport");
	public:
		eTeleportType	type;
		Color			fade_color;
	};
};

#endif//_C_AREA_2D_BASE_H_