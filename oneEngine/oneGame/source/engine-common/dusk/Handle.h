#ifndef ENGINE_COMMON_DUSK_UI_HANDLE_H_
#define ENGINE_COMMON_DUSK_UI_HANDLE_H_

#include "core/math/Rect.h"

namespace Dusk
{
	class Element;
	class UserInterface;

	class Handle
	{
	public:
		ENGCOM_API Handle ( void )
			: m_index(0), m_element(NULL), m_interface(NULL)
			{}
		ENGCOM_API Handle ( int val, UserInterface* ui )
			: m_index(val), m_element(NULL), m_interface(ui)
			{
				updateElement(NULL);
			}

		ENGCOM_API Element*		updateElement( UserInterface* ui );

	public:
		// Treat as index:

		ENGCOM_API Handle&		operator= ( int const &rhs )
			{ m_index = rhs; return *this; }
		ENGCOM_API bool			operator== ( int const &rhs )
			{ return m_index==rhs; }
		ENGCOM_API bool			operator!= ( int const &rhs )
			{ return m_index!=rhs; }
		ENGCOM_API bool			operator>= ( int const &rhs )
			{ return m_index>=rhs; }
		ENGCOM_API bool			operator<= ( int const &rhs )
			{ return m_index<=rhs; }
		ENGCOM_API bool			operator> ( int const &rhs )
			{ return m_index>rhs; }
		ENGCOM_API bool			operator< ( int const &rhs )
			{ return m_index<rhs; }
		ENGCOM_API bool			operator== ( Handle const &rhs )
			{ return m_index==rhs.m_index; }
		ENGCOM_API bool			operator!= ( Handle const &rhs )
			{ return m_index!=rhs.m_index; }
		ENGCOM_API bool			operator>= ( Handle const &rhs )
			{ return m_index>=rhs.m_index; }
		ENGCOM_API bool			operator<= ( Handle const &rhs )
			{ return m_index<=rhs.m_index; }
		ENGCOM_API bool			operator> ( Handle const &rhs )
			{ return m_index>rhs.m_index; }
		ENGCOM_API bool			operator< ( Handle const &rhs )
			{ return m_index<rhs.m_index; }
		ENGCOM_API operator const int32_t() const
			{ return (int32_t)m_index; }
		ENGCOM_API operator const uint32_t() const
			{ return (uint32_t)m_index; }
		ENGCOM_API operator const int64_t() const
			{ return (int64_t)m_index; }
		ENGCOM_API operator const uint64_t() const
			{ return (uint64_t)m_index; }

	public:
		// Treat as pointer:
		ENGCOM_API Element*		operator*();
		ENGCOM_API Element*		operator->();

		template <typename T>
		ENGCOM_API T*			as (void)
			{ return static_cast<T*>(updateElement(NULL)); }

	public:
		// Treat as element:
		/*ENGCOM_API void	SetVisible ( const bool=true );
		ENGCOM_API void	SetRect ( const Rect& );
		ENGCOM_API void	SetText ( const string& );
		ENGCOM_API void	SetParent ( const Handle& );
		ENGCOM_API void	ToggleVisibility ( void );

		ENGCOM_API Rect	GetRect ( void );
		ENGCOM_API bool	GetClicked ( void );
		ENGCOM_API bool	GetMouseOver ( void );
		ENGCOM_API bool	GetButtonClicked ( void );
		ENGCOM_API int	GetDialogueResponse ( void );*/

	private:
		int64_t				m_index;
		int64_t				_padding0;
		Element*			m_element;
		UserInterface*		m_interface;
	};

	static_assert(sizeof(Handle) == sizeof(int64_t)*4, "Expected 256-bit size for Dusk::Handle");
};
// End Namespace

#endif//ENGINE_COMMON_DUSK_UI_HANDLE_H_