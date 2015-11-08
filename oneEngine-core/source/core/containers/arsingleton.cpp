
#include "core/types/types.h"
#include "core/exceptions/exceptions.h"
#include "arsingleton.h"

arsingleton* arsingleton::mActive = NULL;

arsingleton::arsingleton ( void )
{
	if ( mActive != NULL )
	{
		throw Core::InvalidInstantiationException();
	}
	mActive = this;
}
arsingleton::~arsingleton ( void )
{
	if ( mActive != this )
	{
		throw Core::InvalidInstantiationException();
	}
	mActive = NULL;
}
arsingleton* arsingleton::Active ( void )
{
	return mActive;
}