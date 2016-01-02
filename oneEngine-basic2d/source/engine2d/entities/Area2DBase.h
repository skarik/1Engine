
#ifndef _C_AREA_2D_BASE_H_
#define _C_AREA_2D_BASE_H_

#include <vector>

#include "core/math/Vector3d.h"
#include "core/math/Rect.h"
#include "engine/behavior/CGameBehavior.h"

namespace Engine2D
{
	class Area2DBase : public CGameBehavior
	{
		BaseClass("Area2DBase");
		ClassName("Area2DBase");
	public:
		ENGINE2D_API Area2DBase ( void );
		ENGINE2D_API ~Area2DBase ( void );

		ENGINE2D_API void Update ( void ) override
		{
			;
		}

	public:
		// rect that this area covers
		Rect	m_rect;
		
	public:
		//=========================================//
		// Static listing
		//=========================================//
		
		//		Areas ()
		// Returns read-only list of all areas
		ENGINE2D_API static const std::vector<Area2DBase*>& Areas ( void );
	private:
		static std::vector<Area2DBase*>	m_areas;
	};
	typedef Area2DBase Area2D;
}

#endif//_C_AREA_2D_BASE_H_