#include "IPrRigidBody.h"

btRigidBody* IPrRigidBody::NewRigidBody( const btRigidBody::btRigidBodyConstructionInfo& constructionInfo )
{
	return new btRigidBody(constructionInfo);
}

void IPrRigidBody::DeleteRigidBody( btRigidBody** rigidBodyPointer )
{
	if (*rigidBodyPointer != NULL)
	{
		delete *rigidBodyPointer;
		*rigidBodyPointer = NULL;
	}
}