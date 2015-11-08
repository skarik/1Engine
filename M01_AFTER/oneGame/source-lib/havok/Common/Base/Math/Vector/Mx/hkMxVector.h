/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_MXVECTOR_H
#define HK_MXVECTOR_H

namespace hkCompileError
{
	template <bool b> struct MX_SUBVECTOR_INDEX_OUT_OF_RANGE;
	template <> struct MX_SUBVECTOR_INDEX_OUT_OF_RANGE<true>{ };

	template <bool b> struct MX_VECTOR4_SUBVECTOR_INDEX_OUT_OF_RANGE;
	template <> struct MX_VECTOR4_SUBVECTOR_INDEX_OUT_OF_RANGE<true>{ };

	template <bool b> struct MX_UNKNOWN_PERMUTATION;
	template <> struct MX_UNKNOWN_PERMUTATION<true>{ };

	template <bool b> struct MX_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH;
	template <> struct MX_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH<true>{ };

	template <bool b> struct MX_VECTOR4_UNSUPPORTED_VECTOR_LENGTH;
	template <> struct MX_VECTOR4_UNSUPPORTED_VECTOR_LENGTH<true>{ };
}
#define HK_MXVECTOR_VECTOR4_SUBINDEX_CHECK           HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<4), MX_VECTOR4_SUBVECTOR_INDEX_OUT_OF_RANGE)
#define HK_MXVECTOR_MX_SUBINDEX_CHECK                HK_COMPILE_TIME_ASSERT2((I>=0)&&(I<M), MX_SUBVECTOR_INDEX_OUT_OF_RANGE)
#define HK_MXVECTOR_MX_NOT_IMPLEMENTED               HK_COMPILE_TIME_ASSERT2(M==0, MX_NOT_IMPLEMENTED_FOR_THIS_VECTOR_LENGTH)
#define HK_MXVECTOR_VECTOR4_UNSUPPORTED_LENGTH_CHECK HK_COMPILE_TIME_ASSERT2((N>0)&&(N<=4), MX_VECTOR4_UNSUPPORTED_VECTOR_LENGTH)

#define hkMxVectorParameter const hkMxVector<M>&

template <int M> class hkMxReal;
template <int M> class hkMxMask;
#include <Common/Base/Math/Vector/Mx/hkMxReal.h>
#include <Common/Base/Math/Vector/Mx/hkMxSingle.h>
#include <Common/Base/Math/Vector/Mx/hkMxMask.h>

/// Permutations of a hkMxVector vector
struct hkMxVectorPermutation
{
	enum Permutation 
	{
		SHIFT_RIGHT_CYCLIC, ///< Shift the subvectors to the right (higher indices). Example: 0123 -> 3012
		SHIFT_LEFT_CYCLIC,  ///< Shift the subvectors to the left (lower indices). Example: 0123 -> 1230
		REVERSE				///< Reverse the subvector order. Example: 0123 -> 3210
	};
};


/// Long vector for vectorized computing (Mx4 vectors).
///
/// This represents M 4-component vectors. In this documentation we use abcd and following letters to
/// refer to one of the 4-component subvectors.
/// Example M=4: layout of the long vector: abcd efgh ijlk mnop
///
/// The purpose of this vector is to implement efficient loop-unrolled algorithms without branching
/// to efficiently use the SIMD processing hardware capabilities.
template <int M>
class hkMxVector
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MATH, hkMxVector<M>);

	/// The number of 4-component vectors stored in this vector.
	///
	/// When coding, this value can be retrieved using the class definition as well as from variable instances.
	/// Example: incrementing a loop counter: i += hkMxVector<M>::mxLength;.
	/// Example: declaring an array appropriate with same length as a vector: int array[vec_var.mxLength];
	enum { mxLength = M };

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
	/// Default constructor. Does not initialize.
	HK_FORCE_INLINE hkMxVector() { }

	/// Set self to replicate \a v: abcd abcd abcd abcd
	HK_FORCE_INLINE explicit hkMxVector(hkVector4Parameter v);

	/// Set self to broadcast \a r: aaaa aaaa aaaa aaaa
	HK_FORCE_INLINE explicit hkMxVector(hkSimdRealParameter r);

	/// Set self to broadcast \a r: aaaa eeee iiii mmmm
	HK_FORCE_INLINE explicit hkMxVector(hkMxRealParameter r);

#endif

	/// Copies all components from \a v. ( self = v )
	HK_FORCE_INLINE void operator= ( hkMxVectorParameter v );

	/// Set self to broadcast \a r: aaaa eeee iiii mmmm
	HK_FORCE_INLINE void setBroadcast( hkMxRealParameter r );

	/// Read subvectors contiguous from memory as abcd efgh ijkl mnop
	HK_FORCE_INLINE void moveLoad(const hkVector4* HK_RESTRICT v);
	HK_FORCE_INLINE void load(const hkReal* r);
	HK_FORCE_INLINE void loadNotAligned(const hkReal* r);
	HK_FORCE_INLINE void loadNotCached(const hkReal* r);
	/// Store subvectors contiguous to memory \a vOut = abcd efgh ijkl mnop
	HK_FORCE_INLINE void moveStore(hkVector4* HK_RESTRICT vOut) const;
	HK_FORCE_INLINE void store(hkReal* rOut) const;
	HK_FORCE_INLINE void storeNotAligned(hkReal* rOut) const;
	HK_FORCE_INLINE void storeNotCached(hkReal* rOut) const;

	/// Read subvectors non-contiguous from memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkVector4* base);
	/// Read subvectors non-linear indexed from memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkVector4* base, const hkUint16* indices);

	/// Read subvectors non-linear indexed from memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void gather(const hkVector4* base, const hkInt32* indices);

	/// Read subvectors non-linear from memory using addresses \a base[m] + byteAddressOffset
	template <hkUint32 byteAddressOffset> HK_FORCE_INLINE void gatherWithOffset(const void* base[M]);


	/// Write subvectors non-contiguous to memory using addresses \a base + (m * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkVector4* base) const;

	/// Write subvectors non-linear indexed to memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkVector4* base, const hkUint16* indices) const;
	/// Write subvectors non-linear indexed to memory using addresses \a base + ( \a indices[m] * byteAddressIncrement)
	template <hkUint32 byteAddressIncrement> HK_FORCE_INLINE void scatter(hkVector4* base, const hkInt32* indices) const;
	/// Write subvectors non-linear to memory using addresses \a base[m] + byteAddressOffset
	template <hkUint32 byteAddressOffset> HK_FORCE_INLINE void scatterWithOffset(void* base[M]) const;
	
	/// Set the I-th subvector to \a v.
 	template <int I> HK_FORCE_INLINE void setVector(hkVector4Parameter v);

	/// Get the I-th subvector: return [I=0]abcd [I=1]efgh [I=2]ijkl [I=3]mnop
	template <int I> HK_FORCE_INLINE const hkVector4& getVector() const;

	/// Get the I-th subvector: set \a vOut to [I=0]abcd [I=1]efgh [I=2]ijkl [I=3]mnop
	template <int I> HK_FORCE_INLINE void getVector(hkVector4& vOut) const;

	/// Take the I-th component from every subvector in \a v and broadcast within the subvector only. Example: [I=1] gives bbbb ffff jjjj nnnn
	template <int I> HK_FORCE_INLINE void setScalarBroadcast(hkMxVectorParameter v);

	// no operands
	HK_FORCE_INLINE void setZero();
	/// Set component I in every subvector to zero.
	template <int I> HK_FORCE_INLINE void zeroComponent();

	template <int N> HK_FORCE_INLINE void normalize(); // 23bit, sqrt set 0
	template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void normalize();

	// one operand
	HK_FORCE_INLINE void add(hkMxVectorParameter v);
	HK_FORCE_INLINE void sub(hkMxVectorParameter v);
	HK_FORCE_INLINE void mul(hkMxVectorParameter v);
	HK_FORCE_INLINE void setXYZ(hkMxVectorParameter v);
	HK_FORCE_INLINE void setW(hkMxVectorParameter v);
	template <int N> HK_FORCE_INLINE void setNeg(hkMxVectorParameter v0);
	HK_FORCE_INLINE void setAbs(hkMxVectorParameter v0);
	HK_FORCE_INLINE void setSqrtInverse( hkMxVectorParameter v ); // 23 bit, sqrt set 0
	template <hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void setSqrtInverse( hkMxVectorParameter v );

	// two operands
	HK_FORCE_INLINE void setAdd(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setSub(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setMul(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setDiv(hkMxVectorParameter v0, hkMxVectorParameter v1);
	template <hkMathAccuracyMode A, hkMathDivByZeroMode D> HK_FORCE_INLINE void setDiv(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setCross(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setXYZ_W(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setXYZ_W(hkMxVectorParameter v0, hkMxRealParameter v1);
	HK_FORCE_INLINE void setMax(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void setMin(hkMxVectorParameter v0, hkMxVectorParameter v1);

	HK_FORCE_INLINE void addMul(hkMxVectorParameter v0, hkMxVectorParameter v1);
	HK_FORCE_INLINE void addMul(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void addMul(hkMxSingleParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void addMul(hkMxRealParameter v, hkMxVectorParameter s);
	HK_FORCE_INLINE void setAddMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxVectorParameter v2);
	HK_FORCE_INLINE void setAddMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxSingleParameter v2);


	// three operands
	HK_FORCE_INLINE void setSubMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxVectorParameter v2);


	// one single operand
	HK_FORCE_INLINE void add(hkMxSingleParameter s);
	HK_FORCE_INLINE void sub(hkMxSingleParameter s);
	HK_FORCE_INLINE void mul(hkMxSingleParameter s);
	HK_FORCE_INLINE void setW(hkMxRealParameter s);
	HK_FORCE_INLINE void setW(hkMxSingleParameter s);


	// mixed operands

	HK_FORCE_INLINE void mul(hkMxRealParameter s);
	HK_FORCE_INLINE void setMul(hkMxRealParameter v, hkMxVectorParameter s);


	HK_FORCE_INLINE void setAdd(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setSub(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setSub(hkMxSingleParameter s, hkMxVectorParameter v);
	HK_FORCE_INLINE void setMul(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setCross(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setCross(hkMxSingleParameter s, hkMxVectorParameter v);
	HK_FORCE_INLINE void setSubMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxSingleParameter v2);
	HK_FORCE_INLINE void setSubMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxRealParameter   v2);
	HK_FORCE_INLINE void setSubMul(hkMxVectorParameter v0, hkMxSingleParameter v1, hkMxRealParameter   v2);
	HK_FORCE_INLINE void setMax(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setMin(hkMxVectorParameter v, hkMxSingleParameter s);
	HK_FORCE_INLINE void setInterpolate(hkMxVectorParameter v0, hkMxVectorParameter v1, hkMxRealParameter t);


	// comparisons
 	HK_FORCE_INLINE void greater(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greater(hkMxSingleParameter s, hkMxMask<M>& mask) const;
 	HK_FORCE_INLINE void less(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void less(hkMxSingleParameter s, hkMxMask<M>& mask) const;
 	HK_FORCE_INLINE void lessEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void lessEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greaterEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void greaterEqual(hkMxSingleParameter v, hkMxMask<M>& mask) const;
 	HK_FORCE_INLINE void equal(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void equal(hkMxSingleParameter s, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void notEqual(hkMxVectorParameter v, hkMxMask<M>& mask) const;
	HK_FORCE_INLINE void notEqual(hkMxSingleParameter s, hkMxMask<M>& mask) const;

	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxVectorParameter falseValue );
	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxSingleParameter trueValue, hkMxVectorParameter falseValue );
	HK_FORCE_INLINE void setSelect(hkMxMaskParameter mask, hkMxVectorParameter trueValue, hkMxSingleParameter falseValue );

	/// Set to a constant vector.
	template<int vectorConstant> HK_FORCE_INLINE void setConstant();

	/// Add up all subvectors
	HK_FORCE_INLINE void reduceAdd( hkVector4& addOut ) const;

	/// Work on every subvector and return a scalar vector of same length as self
	template <int N> HK_FORCE_INLINE void length( hkMxReal<M>& lensOut ) const; // 23 bit, sqrt set 0
	template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void length( hkMxReal<M>& lensOut ) const;
	template <int N> HK_FORCE_INLINE void lengthSquared( hkMxReal<M>& lensOut ) const;
	template <int N> HK_FORCE_INLINE void lengthInverse( hkMxReal<M>& lensOut ) const; // 23 bit, sqrt set 0
	template <int N, hkMathAccuracyMode A, hkMathNegSqrtMode S> HK_FORCE_INLINE void lengthInverse( hkMxReal<M>& lensOut ) const;
	template <int N> HK_FORCE_INLINE void dot(hkMxSingleParameter v, hkMxReal<M>& dotsOut ) const;
	template <int N> HK_FORCE_INLINE void dot(hkMxVectorParameter v, hkMxReal<M>& dotsOut ) const;

	/// Set self to a permutation of \a v. It is required that self and \a v are non-overlapping.
	template <hkMxVectorPermutation::Permutation P> HK_FORCE_INLINE void setPermutation(hkMxVectorParameter v);
	/// Check whether xyz components of all subvectors are ok.
	/// \warning Expensive function. Use for debugging purposes only.
	template <int N> HK_FORCE_INLINE hkBool32 isOk() const;


	// special methods, which do not exist for all mx lengths
	// return value of special methods

	/// Transpose self and store into \a matrix4. Unused subvectors are assumed zero.
	/// \warning [M:1-4 only] 
	HK_FORCE_INLINE void storeTransposed4(hkMatrix4& matrix4) const;

	/// Add \a v0 to \a v1 scaled by one component of \a v2. Unused subvectors or components are assumed zero. Example: self[1] = v0[1] + (v1[1] * v2.y)
	/// \warning [M:1-4 only] 
	HK_FORCE_INLINE void setAddMul(hkMxVectorParameter v0, hkMxVectorParameter v1, hkVector4Parameter v2); 

	/// Add up all components of each subvector and store into \a addsOut. Unused subvectors or components are assumed zero.
	/// \a addsOut = xyzw with x=a+b+c+d, y=e+f+g+h, z=i+j+k+l, w=m+n+o+p
	/// \warning [M:1-4 only] 
	template <int N> HK_FORCE_INLINE void horizontalAdd( hkVector4& addsOut ) const; 

	/// Calc minimum on every subvector and store into \a minsOut = xyzw with x=min(abcd) y=min(efgh) z=min(ijkl) w=min(mnop)
	/// \warning [M:1-4 only] 
	template <int N> HK_FORCE_INLINE void horizontalMin( hkVector4& minsOut ) const;

	/// Calc maximum on every subvector and store into \a maxsOut = xyzw with x=max(abcd) y=max(efgh) z=max(ijkl) w=max(mnop)
	/// \warning [M:1-4 only] 
	template <int N> HK_FORCE_INLINE void horizontalMax( hkVector4& maxsOut ) const;

	/// Set each subvector of self to a broadcast value of one of the components of \a v: aaaa=x eeee=y iiii=z mmmm=w
	/// creating the same data that a hkMxReal holds virtually
	/// \warning [M:1-4 only] 
	HK_FORCE_INLINE void setAsBroadcast( hkVector4Parameter v );


	hkMxVectorStorage<M> m_vec; ///< The subvectors.
};

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
	#if (HK_SSE_VERSION >= 0x50) && !defined(HK_REAL_IS_DOUBLE)
	#include <Common/Base/Math/Vector/Mx/hkMxVector_AVX.inl>
	#else
	#include <Common/Base/Math/Vector/Mx/hkMxVector.inl>
	#endif
#else
	#include <Common/Base/Math/Vector/Mx/hkMxVector.inl>
#endif

// convenient shortcuts
typedef hkMxVector<4> hk4xVector4;


// helper struct to force code iteration
// use this to create variable repeating code pieces according to
// compiletime constants
// the action to execute in each step needs to expose a member
// 	void step(const int i) {}

template<int START, int END, int STEP = 1>
struct hkMxIterator
{
	template<typename ACTION>
	HK_FORCE_INLINE static void step(ACTION& action) 
	{
		for (int i=START; i!=END; i+=STEP) action.step(i);
	}

	template<typename ACTION>
	HK_FORCE_INLINE static void predicatedStep(ACTION& action, const int LIMIT) 
	{
		for (int i=START; (i!=END) && (i<LIMIT); i+=STEP) action.step(i);
	}
};

template<int STEP>
struct hkMxIterator<0, 1, STEP>
{
	template<typename ACTION>
	HK_FORCE_INLINE static void step(ACTION& action) 
	{
		action.step(0);
	}
};


// helper struct to force code unroll
// use this to create variable repeating code pieces according to
// compiletime constants
// the action to execute in each step needs to expose a template member
// 	template <int I> void step() {}

template<int START, int END, int STEP = 1>
struct hkMxUnroller 
{
	template<typename ACTION>
	HK_FORCE_INLINE static void step(ACTION& action) 
	{
		action.step<START>();
		hkMxUnroller<START+STEP, END, STEP>::step(action);
	}

	template<typename ACTION, int LIMIT>
	HK_FORCE_INLINE static void predicatedStep(ACTION& action) 
	{
		if (START < LIMIT)
		{
			action.step<START>();
			hkMxUnroller<START+STEP, END, STEP>::template predicatedStep<ACTION, LIMIT>(action);
		}
	}
};

template<int END, int STEP>
struct hkMxUnroller<END, END, STEP> 
{
	template<typename ACTION>
	HK_FORCE_INLINE static void step(ACTION& action) { }

	template<typename ACTION, int LIMIT>
	HK_FORCE_INLINE static void predicatedStep(ACTION& action) { }
};



// helper macro to unroll code
// use this to create repeating code with literal available repetition number
// note: you should always prefer the mx classes member functions as they are optimized
//       use this macro only in rare cases as this can generate a lot of code
#	define hkMxUNROLL(x,c) HK_PREPROCESSOR_JOIN_TOKEN(hkMxUNROLL_,x)(c)

#define hkMxUNROLL_0(c)
#define hkMxUNROLL_1(c) hkMxUNROLL_0(c) { const int hkMxI = 0; c; }
#define hkMxUNROLL_2(c) hkMxUNROLL_1(c) { const int hkMxI = 1; c; }
#define hkMxUNROLL_3(c) hkMxUNROLL_2(c) { const int hkMxI = 2; c; }
#define hkMxUNROLL_4(c) hkMxUNROLL_3(c) { const int hkMxI = 3; c; }
#define hkMxUNROLL_5(c) hkMxUNROLL_4(c) { const int hkMxI = 4; c; }
#define hkMxUNROLL_6(c) hkMxUNROLL_5(c) { const int hkMxI = 5; c; }
#define hkMxUNROLL_7(c) hkMxUNROLL_6(c) { const int hkMxI = 6; c; }
#define hkMxUNROLL_8(c) hkMxUNROLL_7(c) { const int hkMxI = 7; c; }
#define hkMxUNROLL_9(c) hkMxUNROLL_8(c) { const int hkMxI = 8; c; }
#define hkMxUNROLL_10(c) hkMxUNROLL_9(c) { const int hkMxI = 9; c; }
#define hkMxUNROLL_11(c) hkMxUNROLL_10(c) { const int hkMxI = 10; c; }
#define hkMxUNROLL_12(c) hkMxUNROLL_11(c) { const int hkMxI = 11; c; }
#define hkMxUNROLL_13(c) hkMxUNROLL_12(c) { const int hkMxI = 12; c; }
#define hkMxUNROLL_14(c) hkMxUNROLL_13(c) { const int hkMxI = 13; c; }
#define hkMxUNROLL_15(c) hkMxUNROLL_14(c) { const int hkMxI = 14; c; }
#define hkMxUNROLL_16(c) hkMxUNROLL_15(c) { const int hkMxI = 15; c; }
#define hkMxUNROLL_17(c) hkMxUNROLL_16(c) { const int hkMxI = 16; c; }
#define hkMxUNROLL_18(c) hkMxUNROLL_17(c) { const int hkMxI = 17; c; }
#define hkMxUNROLL_19(c) hkMxUNROLL_18(c) { const int hkMxI = 18; c; }
#define hkMxUNROLL_20(c) hkMxUNROLL_19(c) { const int hkMxI = 19; c; }
#define hkMxUNROLL_21(c) hkMxUNROLL_20(c) { const int hkMxI = 20; c; }
#define hkMxUNROLL_22(c) hkMxUNROLL_21(c) { const int hkMxI = 21; c; }
#define hkMxUNROLL_23(c) hkMxUNROLL_22(c) { const int hkMxI = 22; c; }
#define hkMxUNROLL_24(c) hkMxUNROLL_23(c) { const int hkMxI = 23; c; }
#define hkMxUNROLL_25(c) hkMxUNROLL_24(c) { const int hkMxI = 24; c; }
#define hkMxUNROLL_26(c) hkMxUNROLL_25(c) { const int hkMxI = 25; c; }
#define hkMxUNROLL_27(c) hkMxUNROLL_26(c) { const int hkMxI = 26; c; }
#define hkMxUNROLL_28(c) hkMxUNROLL_27(c) { const int hkMxI = 27; c; }
#define hkMxUNROLL_29(c) hkMxUNROLL_28(c) { const int hkMxI = 28; c; }
#define hkMxUNROLL_30(c) hkMxUNROLL_29(c) { const int hkMxI = 29; c; }
#define hkMxUNROLL_31(c) hkMxUNROLL_30(c) { const int hkMxI = 30; c; }
#define hkMxUNROLL_32(c) hkMxUNROLL_31(c) { const int hkMxI = 31; c; }
#define hkMxUNROLL_33(c) { HK_MXVECTOR_MX_NOT_IMPLEMENTED; }

#endif // HK_MXVECTOR_H

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
