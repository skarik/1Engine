#ifndef CORE_ACCESSOR_H_
#define CORE_ACCESSOR_H_

#include "core/types/types.h"
#include "core/exceptions/exceptions.h"

#define ARACCESSOR_PRIVATE_EXCLUSIVE_SET(ACCESS_FROM, ACCESSED_INSTANCE_TYPE, FIELD_TYPE, FIELD) \
	class __##ACCESS_FROM##Access_##FIELD \
	{ \
		friend ACCESS_FROM ; \
		ACCESSED_INSTANCE_TYPE * m_target = nullptr; \
		FORCE_INLINE void Set ( const FIELD_TYPE & value ) { m_target-> FIELD = value; } \
		FORCE_INLINE const FIELD_TYPE & Get ( void ) { return m_target-> FIELD ; } \
	public: \
		FORCE_INLINE __##ACCESS_FROM##Access_##FIELD ( ACCESSED_INSTANCE_TYPE * target ) : m_target(target) {} \
	}; \
	FORCE_INLINE __##ACCESS_FROM##Access_##FIELD Access_##FIELD##_From_##ACCESS_FROM ( void ) { return __##ACCESS_FROM##Access_##FIELD ( this ); }

#endif//CORE_ACCESSOR_H_