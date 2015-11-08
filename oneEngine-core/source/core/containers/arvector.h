
#ifndef _C_AR_VECTOR_H_
#define _C_AR_VECTOR_H_

#include "core/types/arBaseObject.h"

template <typename OBJECT>
class arvector : public arBaseObject
{

public:
	
	
public:
	class iterator {
		//operator++
		//operator--
		//operator->
		//operator*
	};

public:
	iterator	begin ( void ) const;
	iterator	end ( void ) const;
	iterator	front ( void ) const;
	iterator	back ( void ) const;
	
	iterator	rbegin ( void ) const;
	iterator	rend ( void ) const;

	void		erase ( const iterator& n_where );

	void		push_back ( const OBJECT& n_object );
	void		pop_back ( void );

	void		reserve ( size_t n_size );
	void		resize ( size_t n_size );

	size_t		size ( void );

public:
	OBJECT&			at ( size_t n_pos );
	const OBJECT&	at ( size_t n_pos ) const;
	OBJECT&			operator[] ( size_t n_pos );
	const OBJECT&	operator[] ( size_t n_pos ) const;

private:
	OBJECT*	m_array;
	size_t	m_array_size;

	size_t	m_full_size;

};

#endif//_C_AR_VECTOR_H_