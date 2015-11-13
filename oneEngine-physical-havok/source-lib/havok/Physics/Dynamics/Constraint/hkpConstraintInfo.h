/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_INFO_H
#define HK_DYNAMICS2_CONSTRAINT_INFO_H
//! Keep the members of this synced with the generic constraint cinfo
//! as it has to save all these members.


	#define HK_SIZE_OF_JACOBIAN_END_SCHEMA 16
	#if HK_POINTER_SIZE == 4
		#define HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA 16
	#else
		#define HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA 32
	#endif
	#define HK_SIZE_OF_JACOBIAN_GOTO_SCHEMA 16
	#define HK_SIZE_OF_JACOBIAN_SHIFT_SOLVER_RESULTS_SCHEMA 16

#if defined(HK_REAL_IS_DOUBLE)
	#define HK_SIZE_OF_JACOBIAN_1D_BILATERAL_SCHEMA (48*2)
	#define HK_SIZE_OF_JACOBIAN_1D_BILATERAL_USER_TAU_SCHEMA (64*2)
	#define HK_SIZE_OF_JACOBIAN_1D_LINEAR_LIMIT_SCHEMA (64*2)
	#define HK_SIZE_OF_JACOBIAN_1D_FRICTION_SCHEMA (64*2)
	#define HK_SIZE_OF_JACOBIAN_1D_LINEAR_MOTOR_SCHEMA (80*2)

	#define HK_SIZE_OF_STABLE_BALL_SOCKET_SCHEMA			(64*2)
	#define HK_SIZE_OF_NP_STABLE_BALL_SOCKET_SCHEMA (HK_SIZE_OF_STABLE_BALL_SOCKET_SCHEMA+32*4)
	#define HK_SIZE_OF_JACOBIAN_1D_PULLEY_SCHEMA (64*2)

	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_SCHEMA (32*2)
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_LIMITS_SCHEMA (48*2)
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_FRICTION_SCHEMA (48*2)
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_MOTOR_SCHEMA (128)

	#define HK_SIZE_OF_JACOBIAN_SINGLE_CONTACT_SCHEMA (48*2)
	#define HK_SIZE_OF_JACOBIAN_PAIR_CONTACT_SCHEMA (112*2)
	#define HK_SIZE_OF_JACOBIAN_2D_FRICTION_SCHEMA (112*2)
#	if HK_POINTER_SIZE == 4
	#define HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA (144*2)
	#define HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA (80*2)
#	else
	#define HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA (160*2)
	#define HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA (96*2)
#	endif
	#define HK_SIZE_OF_JACOBIAN_SET_MASS_SCHEMA (32*2)
	#define HK_SIZE_OF_JACOBIAN_ADD_VELOCITY_SCHEMA (64)
	#define HK_SIZE_OF_JACOBIAN_SET_CENTER_OF_MASS_SCHEMA (96*2)
	#define HK_SIZE_OF_JACOBIAN_STIFF_SPRING_CHAIN_SCHEMA (32)
	#define HK_SIZE_OF_JACOBIAN_BALL_SOCKET_CHAIN_SCHEMA (16*2)
	#define HK_SIZE_OF_JACOBIAN_POWERED_CHAIN_SCHEMA  (32+32)
#else
	#define HK_SIZE_OF_JACOBIAN_1D_BILATERAL_SCHEMA 48
	#define HK_SIZE_OF_JACOBIAN_1D_BILATERAL_USER_TAU_SCHEMA 64
	#define HK_SIZE_OF_JACOBIAN_1D_LINEAR_LIMIT_SCHEMA 64
	#define HK_SIZE_OF_JACOBIAN_1D_FRICTION_SCHEMA 64
	#define HK_SIZE_OF_JACOBIAN_1D_LINEAR_MOTOR_SCHEMA 80

	#define HK_SIZE_OF_STABLE_BALL_SOCKET_SCHEMA 64
	#define HK_SIZE_OF_NP_STABLE_BALL_SOCKET_SCHEMA (HK_SIZE_OF_STABLE_BALL_SOCKET_SCHEMA+16*4)
	#define HK_SIZE_OF_JACOBIAN_1D_PULLEY_SCHEMA 64

	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_SCHEMA 32
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_LIMITS_SCHEMA 48
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_FRICTION_SCHEMA 48
	#define HK_SIZE_OF_JACOBIAN_1D_ANGULAR_MOTOR_SCHEMA 64

	#define HK_SIZE_OF_JACOBIAN_SINGLE_CONTACT_SCHEMA 48
	#define HK_SIZE_OF_JACOBIAN_PAIR_CONTACT_SCHEMA 112
	#define HK_SIZE_OF_JACOBIAN_2D_FRICTION_SCHEMA 112
#	if HK_POINTER_SIZE == 4
	#define HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA 144
	#define HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA 80
#	else
	#define HK_SIZE_OF_JACOBIAN_3D_FRICTION_SCHEMA 160
	#define HK_SIZE_OF_JACOBIAN_2D_ROLLING_FRICTION_SCHEMA 96
#	endif
	#define HK_SIZE_OF_JACOBIAN_SET_MASS_SCHEMA 32



	#define HK_SIZE_OF_JACOBIAN_ADD_VELOCITY_SCHEMA 32
	#define HK_SIZE_OF_JACOBIAN_SET_CENTER_OF_MASS_SCHEMA 96
	#define HK_SIZE_OF_JACOBIAN_STIFF_SPRING_CHAIN_SCHEMA 32
	#define HK_SIZE_OF_JACOBIAN_BALL_SOCKET_CHAIN_SCHEMA 32
	#define HK_SIZE_OF_JACOBIAN_POWERED_CHAIN_SCHEMA  32
#endif

#define hkpConstraintInfoSpu2 hkpConstraintInfo


/// Contains information about the memory size required for jacobians (m_sizeOfSchemas) and the number of solver results (m_numSolverResults)
/// and temporal elements (m_numSolverElemTemps) for an associated constraint (hkpConstraintData)
struct hkpConstraintInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DYNAMICS, hkpConstraintInfo );

		// This variable only has a meaning in an island (it is equal to sizeOfJacobians for constraints).
		// It relates to splitting buffers on playstation
	int m_maxSizeOfSchema;
	int m_sizeOfSchemas;
	int m_numSolverResults;
	int m_numSolverElemTemps;

	inline void clear( ) { m_maxSizeOfSchema = 0; m_sizeOfSchemas = 0; m_numSolverResults = 0; m_numSolverElemTemps = 0; }
	inline void addHeader( ) { m_sizeOfSchemas += HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA; }
	inline void add( int schemaSize, int numSolverResults, int numSolverTempElems ) { m_sizeOfSchemas += schemaSize; m_numSolverResults += numSolverResults; m_numSolverElemTemps += numSolverTempElems; }

	inline void add( const hkpConstraintInfo& other);
	inline void sub( const hkpConstraintInfo& other);

	inline void merge( const hkpConstraintInfo& other);

#if !defined(hkpConstraintInfoSpu2)
	inline hkpConstraintInfo& operator= (const struct hkpConstraintInfoSpu2& s);
#endif
};


inline void hkpConstraintInfo::merge( const hkpConstraintInfo& delta)
{
	m_maxSizeOfSchema = hkMath::max2( m_maxSizeOfSchema, delta.m_maxSizeOfSchema);
	m_sizeOfSchemas    += delta.m_sizeOfSchemas;
	m_numSolverResults += delta.m_numSolverResults;
	m_numSolverElemTemps += delta.m_numSolverElemTemps;
}


inline void hkpConstraintInfo::add( const hkpConstraintInfo& delta)
{
	//	HK_ASSERT2(0XAD7865543, m_maxSizeOfSchema < 10000 && delta.m_maxSizeOfSchema < 10000 && delta.m_sizeOfSchemas  < 10000, "Warning: Max schema size very (too?) large.");
	m_maxSizeOfSchema = hkMath::max2( m_maxSizeOfSchema, int(delta.m_maxSizeOfSchema));
	m_maxSizeOfSchema = hkMath::max2( m_maxSizeOfSchema, int(delta.m_sizeOfSchemas));
	m_sizeOfSchemas    += delta.m_sizeOfSchemas;
	m_numSolverResults += delta.m_numSolverResults;
	m_numSolverElemTemps += delta.m_numSolverElemTemps;
}

inline void hkpConstraintInfo::sub( const hkpConstraintInfo& delta)
{
	m_sizeOfSchemas    -= delta.m_sizeOfSchemas;
	m_numSolverResults -= delta.m_numSolverResults;
	m_numSolverElemTemps -= delta.m_numSolverElemTemps;
}


#if !defined(hkpConstraintInfoSpu2)
struct hkpConstraintInfoSpu2
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DYNAMICS, hkpConstraintInfo );

	// This variable only has a meaning in an island (it is equal to sizeOfJacobians for constraints).
	// It relates to splitting buffers on playstation
	hkPadSpu<int> m_maxSizeOfSchema;
	hkPadSpu<int> m_sizeOfSchemas;
	hkPadSpu<int> m_numSolverResults;
	hkPadSpu<int> m_numSolverElemTemps;

	inline void clear( ) { m_maxSizeOfSchema = 0; m_sizeOfSchemas = 0; m_numSolverResults = 0; m_numSolverElemTemps = 0; }
	inline void addHeader( ) { m_sizeOfSchemas = m_sizeOfSchemas + HK_SIZE_OF_JACOBIAN_HEADER_SCHEMA; }
	inline void add( int schemaSize, int numSolverResults, int numSolverTempElems )
	{
		m_sizeOfSchemas = m_sizeOfSchemas + schemaSize;
		m_numSolverResults = m_numSolverResults + numSolverResults;
		m_numSolverElemTemps = m_numSolverElemTemps + numSolverTempElems;
	}

	inline void add( const hkpConstraintInfo& other);
	inline void sub( const hkpConstraintInfo& other);
};

inline void hkpConstraintInfoSpu2::add( const hkpConstraintInfo& delta)
{
	//	HK_ASSERT2(0XAD7865543, m_maxSizeOfSchema < 10000 && delta.m_maxSizeOfSchema < 10000 && delta.m_sizeOfSchemas  < 10000, "Warning: Max schema size very (too?) large.");
	m_maxSizeOfSchema = hkMath::max2( int(m_maxSizeOfSchema), int(delta.m_maxSizeOfSchema));
	m_maxSizeOfSchema = hkMath::max2( int(m_maxSizeOfSchema), int(delta.m_sizeOfSchemas));
	m_sizeOfSchemas      = m_sizeOfSchemas + delta.m_sizeOfSchemas;
	m_numSolverResults   = m_numSolverResults + delta.m_numSolverResults;
	m_numSolverElemTemps = m_numSolverElemTemps + delta.m_numSolverElemTemps;
}

inline void hkpConstraintInfoSpu2::sub( const hkpConstraintInfo& delta)
{
	m_sizeOfSchemas      = m_sizeOfSchemas - delta.m_sizeOfSchemas;
	m_numSolverResults   = m_numSolverResults - delta.m_numSolverResults;
	m_numSolverElemTemps = m_numSolverElemTemps - delta.m_numSolverElemTemps;
}

inline hkpConstraintInfo& hkpConstraintInfo::operator= (const struct hkpConstraintInfoSpu2& s)
{
	m_maxSizeOfSchema = s.m_maxSizeOfSchema;
	m_sizeOfSchemas = s.m_sizeOfSchemas;
	m_numSolverResults = s.m_numSolverResults;
	m_numSolverElemTemps = s.m_numSolverElemTemps;
	return *this;
}

#endif


#endif // HK_DYNAMICS2_CONSTRAINT_DATA_H

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20120119)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2012
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */
