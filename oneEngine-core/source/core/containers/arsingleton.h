// arsinglton.h
// Singleton pattern base class.
// Also provides macro definitions for building a singleton class

#ifndef _C_AR_SINGLETON_H_
#define _C_AR_SINGLETON_H_

class arsingleton
{
public:
	arsingleton ( void );
	virtual ~arsingleton ( void );
	static arsingleton* Active ( void );
private:
	static arsingleton*	mActive;
};

// Singleton Header declaration
#define ARSINGLETON_DECLARATION (T) \
	private: \
		static T* mActive; \
	public: \
		static T* Active ( void ); \
		void _init ( void ); \
		void _free ( void );

// Singleton CPP definition
#define ARSINGLETON_DEFINITION (T) \
	T* T::mActive = NULL; \
	T::_init ( void ) \
	{ \
		if ( mActive != NULL ) \
			throw Core::InvalidInstantiationException(); \
		mActive = this; \
	} \
	T::_free ( void ) \
	{ \
		if ( mActive != this ) \
			throw Core::InvalidInstantiationException(); \
		mActive = NULL; \
	} \
	T* T::Active ( void ) \
	{ \
		return mActive; \
	} 

#endif//_C_AR_SINGLETON_H_