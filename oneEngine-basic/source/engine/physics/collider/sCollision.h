

#ifndef _S_COLLISION_
#define _S_COLLISION_

#include "core/math/Vector3d.h"
#include "core/math/Ray.h"

class CCollider;
class CGameBehavior;
class IPrGameMotion;

struct sCollision
{
	//CCollider*	m_collider_Other;
	//CCollider*	m_collider_This;
	IPrGameMotion*	m_motion_Other;
	IPrGameMotion*	m_motion_This;

	CGameBehavior*	m_hit_Other;
	CGameBehavior*	m_hit_This;
	//Vector3d	vPos;
	Ray			m_hit;
};

#endif