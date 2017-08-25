
#ifndef _C_AREA_2D_TRIGGER_H_
#define _C_AREA_2D_TRIGGER_H_

#include "core/math/Color.h"

#include "Area2DBase.h"

namespace Engine2D
{
	class AreaTrigger : public Area2DBase
	{
		ClassName("AreaTrigger");
	public:
		arstring<128>	m_target_actor_type;
		arstring<128>	m_target_actor_name;
	};
};

#endif//_C_AREA_2D_TRIGGER_H_