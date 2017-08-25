
#ifndef _C_AREA_2D_TELEPORT_H_
#define _C_AREA_2D_TELEPORT_H_

#include "core/math/Color.h"

#include "Area2DBase.h"

namespace Engine2D
{
	enum class eTeleportType : uint8_t
	{
		EdgeOrNormal		= 0,
		Boring				= 1,
		Glitch				= 2
	};

	class AreaTeleport : public Area2DBase
	{
		ClassName("AreaTeleport");
	public:
		eTeleportType	type;
		Color			fade_color;
		arstring<128>	target_room;
		arstring<128>	target_area;
	};
};

#endif//_C_AREA_2D_BASE_H_