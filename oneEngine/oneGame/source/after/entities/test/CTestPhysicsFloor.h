

#ifndef ____CTESTFLOOR_____
#define ____CTESTFLOOR_____

#include "engine/behavior/CGameObject.h"

class CBoxCollider;
class CRigidBody;
class CCubeRenderablePrimitive;

class CTestPhysicsFloor : public CGameObject
{
public:
	CTestPhysicsFloor();
	~CTestPhysicsFloor();

	void Update ( void );

	CCubeRenderablePrimitive* pRenderable;
protected:
	CBoxCollider* pCollider;
	CRigidBody* pRigidbody;

};

#endif