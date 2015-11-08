/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_MATH_VECTOR4_COMPARISON_H
#define HK_MATH_VECTOR4_COMPARISON_H

#ifndef HK_MATH_MATH_H
#	error Please include Common/Base/hkBase.h instead of this file.
#endif


/// \class hkVector4Comparison
///
/// A mask for floating point comparisons. It holds a special representation for a boolean 
/// for every component of a hkVector4 or hkSimdReal. There are optimized SIMD implementations
/// available for several platforms. They use platform specific data structures and
/// code to provide efficient processing. Thus to be portable, nothing should be assumed
/// about the internal boolean value or layout of the storage.
///
/// This class also defines a list of platform independent symbols in 'Mask' which should
/// exclusively be used in code to hand-craft known masks. For compatibility with possible
/// future extensions, nothing should be assumed about the value of these symbols.
///
/// This mask can hold up to four comparison results and can be used with both hkVector4 and
/// hkSimdReal interchangeably. 
///
/// \sa hkVector4 hkSimdReal
class hkVector4Comparison
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkVector4Comparison);

	/// Defines the various mask combinations possible.
	/// This is a platform independent description of possible outcomes of compare operations.
	/// Although these symbols are assigned to values here, nothing should be assumed about the
	/// bit pattern employed. It is not guaranteed to be invariant over releases.
	enum Mask
	{
		INDEX_W		= 3,	// Index of the w component
		INDEX_Z		= 2,
		INDEX_Y		= 1,
		INDEX_X		= 0,

		MASK_NONE	= 0x0,					// 0000
		MASK_W		= (1 << INDEX_W),		// 0001
		MASK_Z		= (1 << INDEX_Z),		// 0010
		MASK_ZW		= (MASK_Z | MASK_W),	// 0011

		MASK_Y		= (1 << INDEX_Y),		// 0100
		MASK_YW		= (MASK_Y | MASK_W),	// 0101
		MASK_YZ		= (MASK_Y | MASK_Z),	// 0110
		MASK_YZW	= (MASK_YZ | MASK_W),	// 0111

		MASK_X		= (1 << INDEX_X),		// 1000
		MASK_XW		= (MASK_X | MASK_W),	// 1001
		MASK_XZ		= (MASK_X | MASK_Z),	// 1010
		MASK_XZW	= (MASK_XZ | MASK_W),	// 1011

		MASK_XY		= (MASK_X | MASK_Y),	// 1100
		MASK_XYW	= (MASK_XY | MASK_W),	// 1101
		MASK_XYZ	= (MASK_XY | MASK_Z),	// 1110
		MASK_XYZW	= (MASK_XY | MASK_ZW)	// 1111
	};

	/// Static conversion method to create a hkVector4Comparison from a platform dependent comparison mask \a x. ( self.storage = x ).
	/// This method is implemented platform-specific to avoid constructor initialization overhead
	/// or read-modify-write data dependencies which might occur.
	HK_FORCE_INLINE static const hkVector4Comparison HK_CALL convert(const hkVector4Mask& x);

	/// Static helper method which returns the appropriate mask for the specified component \a i. ( return self[i] ).
	/// \remark Use this method only when the component index is not a compile time constant.
	HK_FORCE_INLINE static Mask HK_CALL getMaskForComponent(int i);

	/// Return the index of the last component set in self. If no component is set, zero is returned.
	HK_FORCE_INLINE int getIndexOfLastComponentSet() const;

	/// Return the index of the first component set in self. If no component is set, zero is returned.
	HK_FORCE_INLINE int getIndexOfFirstComponentSet() const;

	/// Sets the comparison mask to be the component-wise logical 'and' of \a a and \a b. ( self = a && b )
	HK_FORCE_INLINE void setAnd( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b );

	/// Sets the comparison mask to the component-wise logical 'and not' of \a a and \a b. ( self = a && !b )
	HK_FORCE_INLINE void setAndNot( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b );

	/// Sets the comparison mask to the component-wise logical 'or' of \a a and \a b. ( self = a || b )
	HK_FORCE_INLINE void setOr( hkVector4ComparisonParameter a, hkVector4ComparisonParameter b );

	/// Sets the comparison mask to the component-wise logical 'not' of \a a. ( self = !a )
	HK_FORCE_INLINE void setNot( hkVector4ComparisonParameter a );

	/// Component-wise select mask values from \a trueValue or \a falseValue depending on whether
	/// the component is marked set or clear in the \a comp mask. ( self = comp ? trueValue : falseValue )
	HK_FORCE_INLINE void setSelect( hkVector4ComparisonParameter comp, hkVector4ComparisonParameter trueValue, hkVector4ComparisonParameter falseValue );

	/// Set self to form the mask \a m. ( self.storage = m ).
	/// \remark Use this method only when the component mask is not a compile time constant.
	HK_FORCE_INLINE void set( Mask m );

	/// Set self to form the mask \a M. ( self.storage = m )
	template <Mask M> HK_FORCE_INLINE void set();

	/// Returns true if every component which is selected in the mask \a m is also set
	/// in self. Returns false otherwise. ( return ( self && m ) == m ).
	/// \remark If MASK_NONE is passed in as a parameter this method will always return true.
	HK_FORCE_INLINE hkBool32 allAreSet( Mask m ) const;

	/// Returns true only if all components of self are set. ( return ( self && m ) == ALL )
	HK_FORCE_INLINE hkBool32 allAreSet() const;

	/// Returns true if any component which is selected in the mask \a m is also set
	/// in self. Returns false otherwise. ( return ( self && m ) != NONE ).
	/// \remark If MASK_NONE is passed in as a parameter this method will always return false.
	HK_FORCE_INLINE hkBool32 anyIsSet( Mask m ) const;

	/// Returns true as soon as any component of self is set. ( return self != NONE )
	HK_FORCE_INLINE hkBool32 anyIsSet() const;

	/// Returns a mask describing which components of self are set. ( return self.storage )
	HK_FORCE_INLINE Mask getMask() const;

	/// Returns a mask describing which of the components of self, selected by \a m, are set. ( return self.storage && m )
	HK_FORCE_INLINE Mask getMask(Mask m) const;

	/// Static method to return a 32 bit integer holding the combined masks of \a ca, \a cb and \a cc.
	/// The mask of \a ca is returned in Bits [0-7], the mask of \a cb in [8-15] and the mask of \a cc in [16-23].
	static HK_FORCE_INLINE hkUint32 HK_CALL getCombinedMask(hkVector4ComparisonParameter ca, hkVector4ComparisonParameter cb, hkVector4ComparisonParameter cc );


	/// Internal data storage of the boolean value components (platform dependent).
	/// For writing portable code, nothing can be assumed about the internal layout of the values.
	hkVector4Mask m_mask;

	protected:

	static const hkUint8 s_maskToFirstIndex[16]; ///< selection map to lowest index set
	static const hkUint8 s_maskToLastIndex[16];  ///< selection map to highest index set
};

#endif //HK_MATH_VECTOR4_COMPARISON_H

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
