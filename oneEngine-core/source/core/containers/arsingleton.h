//===============================================================================================//
//
//		arsingleton.h
//
// Provides macro definitions for a singleton class.
// Because of the behavior of singleton templates in the linker, they are not provided.
//
//===============================================================================================//
#ifndef CORE_SINGLETON_H_
#define CORE_SINGLETON_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"

//===============================================================================================//
// 1. Place the following in the header
//===============================================================================================//

// Singleton header storage definition. Place into class declaration:
#define ARSINGLETON_H_STORAGE(T,STORAGE) \
	private: \
		STORAGE static T * m_Active; \
		void _free ( void ) \
		{ \
			if ( m_Active != this ) \
				throw core::InvalidInstantiationException(); \
			m_Active = NULL; \
		} 
// Singleton header storage definition. Place into class declaration:
#define ARSINGLETON_H_ACCESS(T) \
	public: \
		static T * Active ( void ) \
		{ \
			if ( m_Active == NULL ) { \
				m_Active = new T(); \
			} \
			return m_Active; \
		} 

//===============================================================================================//
// 2. Place the following in the source implementation
//===============================================================================================//

// Singleton CPP definition. Place into class definition file in source:
#define ARSINGLETON_CPP_DEF(T) T* T::m_Active = NULL; 

#endif//CORE_SINGLETON_H_