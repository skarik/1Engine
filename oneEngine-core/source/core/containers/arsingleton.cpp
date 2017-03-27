
#include "core/types/types.h"
#include "core/exceptions/exceptions.h"
#include "arsingleton.h"

arsingleton* arsingleton::mActive = NULL;

arsingleton::arsingleton ( void )
{
	if ( mActive != NULL )
	{
		throw core::InvalidInstantiationException();
	}
	mActive = this;
}
arsingleton::~arsingleton ( void ) throw(...)
{
	if ( mActive != this )
	{
		throw core::InvalidInstantiationException();
	}
	mActive = NULL;
}
arsingleton* arsingleton::Active ( void )
{
	return mActive;
}