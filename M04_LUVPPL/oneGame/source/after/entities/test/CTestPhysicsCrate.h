

#ifndef ____CTESTCRATE_____
#define ____CTESTCRATE_____

#include "engine/behavior/CGameObject.h"

class CBoxCollider;
class CRigidBody;
class CCubeRenderablePrimitive;

class CTestPhysicsCrate : public CGameObject
{
public:
	CTestPhysicsCrate();
	~CTestPhysicsCrate();

	void FixedUpdate ( void );

protected:
	CBoxCollider* pCollider;
	CRigidBody* pRigidbody;
	CCubeRenderablePrimitive* pRenderable;

};

#endif//____CTESTCRATE_____