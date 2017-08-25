//===============================================================================================//
//
//		class dirtval
//
// Template class that wraps a type with an additional boolean value representing external change.
// May be queried if the type was changed. Otherwise, acts as the underlying type.
//
//===============================================================================================//
#ifndef CORE_EXT_DIRT_VAL_H_
#define CORE_EXT_DIRT_VAL_H_

#include "core/math/Vector3d.h"

template <typename Type>
class dirtval
{
public:
	explicit dirtval ( void )
		: m_dirty(false), m_val()
	{
	}
	explicit dirtval ( const Type& n_val )
		: m_dirty(false), m_val(n_val)
	{
	}

	operator const Type&() const {
		return m_val;
	} 
	Type& operator= ( const Type&rhs ) {
		m_dirty = true;
		m_val = rhs;
		return m_val;
	}

	const bool isDirty ( void ) const {
		return m_dirty;
	}
	void clean ( void ) {
		m_dirty = false;
	}
private:
	Type m_val;
	bool m_dirty;
};

template <>
class dirtval <Vector3d>
{
public:
	explicit dirtval ( void )
		: m_dirty(false), m_val()
	{
	}
	explicit dirtval ( const Vector3d& n_val )
		: m_dirty(false), m_val(n_val)
	{
	}

	operator const Vector3d&() const {
		return m_val;
	} 
	Vector3d& operator= ( const Vector3d&rhs ) {
		m_dirty = true;
		m_val = rhs;
		return m_val;
	}

	Vector3d& operator- ( const Vector3d&rhs ) const {
		return ( m_val - rhs );
	}
	Vector3d& operator+ ( const Vector3d&rhs ) const {
		return ( m_val + rhs );
	}

	const bool isDirty ( void ) const {
		return m_dirty;
	}
	void clean ( void ) {
		m_dirty = false;
	}
private:
	Vector3d m_val;
	bool m_dirty;
};

#endif//CORE_EXT_DIRT_VAL_H_