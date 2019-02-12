
#ifndef _C_POINT_BASE_H_
#define _C_POINT_BASE_H_

#include "core/math/Vector3.h"
#include "engine/behavior/CGameBehavior.h"

class CPointBase : public CGameBehavior
{
	BaseClass("PointBase");
	ClassName("PointBase");
public:
	CPointBase ( void ) : CGameBehavior()
	{
		layer = physical::layer::Points;
	}

	void Update ( void ) override
	{
		;
	}

public:
	Vector3f	m_position;
};


#endif//_C_POINT_BASE_H_