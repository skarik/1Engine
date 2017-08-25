
#ifndef _C_TEST_PHYSICS_FLARE_H_
#define _C_TEST_PHYSICS_FLARE_H_

#include "CGameObject.h"


//#include "CConvexCollider.h"
#include "CCapsuleCollider.h"
#include "CRigidBody.h"
#include "CModel.h"

class CTestPhysicsFlare : public CGameObject
{
public:
	CTestPhysicsFlare() : CGameObject()
	{	
		pMyModel = new CModel ( string("models/flare01.FBX") );
		pMyModel->transform.SetParent( &transform );
		glMaterial* flareMaterial = new glMaterial;
		flareMaterial->emissive = Color( 0,0,0.0f );
		flareMaterial->diffuse = Color( 1,1,1.0f );
		flareMaterial->loadTexture( ".res/textures/flare01.tga" );
		pMyModel->SetMaterial( flareMaterial );
		flareMaterial->releaseOwnership();

		//pCollider = new CBoxCollider ( Vector3d( 1,1,1 ) );
		//pCollider = new CConvexCollider( string("models\\flare01.FBX") );
		pCollider = new CCapsuleCollider( 1.2f,0.2f, true );

		pRigidbody = new CRigidBody ( pCollider, this );
	}
	~CTestPhysicsFlare()
	{
		delete pMyModel;
		delete pRigidbody;
		delete pCollider;
	}

	void FixedUpdate ( void )
	{
		pRigidbody->ApplyForce( Vector3d( 0.01f, 0,0 ) );
	}

protected:
	CCollider* pCollider;
	CRigidBody* pRigidbody;
	CModel* pMyModel;

};

#endif