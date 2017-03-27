// arsinglton.h
// Singleton pattern base class.
// Also provides macro definitions for building a singleton class

#ifndef _C_AR_SINGLETON_H_
#define _C_AR_SINGLETON_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"

class arsingleton
{
public:
	arsingleton ( void );
	virtual ~arsingleton ( void ) throw(...);
	static arsingleton* Active ( void );
private:
	static arsingleton*	mActive;
};

// Singleton Header declaration
#define ARSINGLETON_H_STORAGE(T,STORAGE) \
	private: \
		STORAGE static T * mActive; \
		void _free ( void ) \
		{ \
			if ( mActive != this ) \
				throw core::InvalidInstantiationException(); \
			mActive = NULL; \
		} 
#define ARSINGLETON_H_ACCESS(T) \
	public: \
		static T * Active ( void ) \
		{ \
			if ( mActive == NULL ) { \
				mActive = new T(); \
			} \
			return mActive; \
		} 

// Singleton CPP definition
#define ARSINGLETON_CPP_DEF(T) T* T::mActive = NULL; 

#endif//_C_AR_SINGLETON_H_