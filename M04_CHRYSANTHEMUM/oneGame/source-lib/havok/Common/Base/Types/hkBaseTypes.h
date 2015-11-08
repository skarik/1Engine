/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKBASE_HKBASETYPES_H
#define HKBASE_HKBASETYPES_H

// Uncomment the next line if you want to build with double precision math
//#define HK_REAL_IS_DOUBLE

//
// compiler
//
#if defined(__GNUC__) || defined(__CC_ARM) || defined(__ARMCC__) || defined(__ghs__)
#	define HK_COMPILER_GCC // ARMCC and GCC are very similar so often can treat them the same.
#	if defined(__CC_ARM) || defined(__ARMCC__)
#		define HK_COMPILER_ARMCC
#   elif defined(__ghs__)
#		define HK_COMPILER_GHS
#	endif
#elif defined(_MSC_VER) // Intel compiler defines this too (most of the time, not on LRB etc though)
#	define HK_COMPILER_MSVC
#elif defined(__MWERKS__)
#	define HK_COMPILER_MWERKS
#else
#	error Could not detect compiler
#endif

//
// architecture
//
#if defined(__i386__) || defined(_M_IX86)
#	define HK_ARCH_IA32
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 4
#elif defined(_M_AMD64) || defined(_M_X64) || defined(__amd64) || defined(__x86_64)
#	define HK_ARCH_X64
#	define HK_ENDIAN_LITTLE 1
#	define HK_ENDIAN_BIG	0
#	define HK_POINTER_SIZE 8
#elif defined(_PPC_) || defined(__POWERPC__) || defined(_M_PPC) || defined(_M_PPCBE) || defined(GEKKO) || defined(EPPC)
#	define HK_ARCH_PPC
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#	define HK_POINTER_SIZE 4
#elif defined(__PPU__) && defined(__CELLOS_LV2__)
#	define HK_ARCH_PS3
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#elif defined(__SPU__) && defined(__CELLOS_LV2__)
#	define HK_ARCH_PS3SPU
#	define HK_ENDIAN_LITTLE 0
#	define HK_ENDIAN_BIG	1
#elif defined(arm) || defined(__arm__)
#   define HK_ARCH_ARM
#   if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7R__) || defined (__ARM_ARCH_7M__)
#       define HK_ARCH_ARM_V7 1
#   elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6T2__) || defined(NN_PROCESSOR_ARM) || defined(NN_PROCESSOR_ARM_V6) 
#       define HK_ARCH_ARM_V6 1
#	else 
#		error Unexpected ARM arch
#   endif
#   if defined(__ARMEB__)
#	   define HK_ENDIAN_LITTLE 0
#	   define HK_ENDIAN_BIG	1
#   else 
#	   define HK_ENDIAN_LITTLE 1
#	   define HK_ENDIAN_BIG	0
#   endif
#	define HK_POINTER_SIZE 4
#	define HK_NATIVE_ALIGN_CHECK 0x7 // ptr & 0x7 == 0 for 8 bytes. This works for runtime, but Havok serialization currentlty assumes 16 for some stuff..
#else
#	error Could not autodetect target architecture
#endif


#ifndef HK_NATIVE_ALIGN_CHECK 
#define HK_NATIVE_ALIGN_CHECK 0xf // ptr & 0xf == 0 for 16 bytes
#endif
//
// platform
//
#	define HK_PLATFORM_WIN32
#	if defined(_WIN64)
#		define HK_PLATFORM_X64
#	endif
#	define HK_PLATFORM_IS_CONSOLE 0

#	define HK_ALIGN_OF(T) __alignof(T)

//
// types
//

/// hkFloat is provided if floats are explicitly required.
typedef float  hkFloat32;
/// hkDouble is provided if doubles are explicit required.
typedef double hkDouble64;

/// Signed 8 bit integer
typedef signed char		hkChar;
/// Signed 8 bit integer
typedef signed char		hkInt8;
/// Signed 16 bit integer
typedef signed short	hkInt16;
/// Signed 32 bit integer
typedef signed int		hkInt32;

/// Unsigned 8 bit integer
typedef unsigned char	hkUchar;
/// Unsigned 8 bit integer
typedef unsigned char	hkUint8;
/// Unsigned 16 bit integer
typedef unsigned short	hkUint16;
/// Unsigned 32 bit integer
typedef unsigned int	hkUint32;

/// hkReal is the default floating point type.
#if defined(HK_REAL_IS_DOUBLE)
typedef hkDouble64 hkReal;
#define HK_ALIGN_REAL HK_ALIGN32
#else
typedef hkFloat32 hkReal;
#define HK_ALIGN_REAL HK_ALIGN16
#endif

#if defined(HK_ARCH_X64)
#	if defined(HK_PLATFORM_X64)
		typedef unsigned __int64 hkUlong;  // MS- specific
		typedef signed __int64 hkLong;  // MS- specific
#	else
		typedef unsigned long hkUlong; // LINUX64
		typedef signed long hkLong; // LINUX64
#	endif
#elif defined(HK_COMPILER_MSVC) && (_MSC_VER >= 1300)
	typedef unsigned long __w64 hkUlong; // VC7.0 or higher, 64bit warnings
	typedef signed long __w64 hkLong; 
#else
	typedef unsigned long hkUlong;
	typedef signed long hkLong;
#endif


#define HK_CPU_PTR( A ) A

	typedef void* hk_va_list;

/// a simple success/failure enum.
enum hkResult
{
	HK_SUCCESS = 0,
	HK_FAILURE = 1
};


//
// useful macros
//

#if  defined(DEBUG) || defined(_DEBUG) || defined(HK_DEBUG)
#	undef HK_DEBUG
#	define HK_DEBUG
#	define HK_ON_DEBUG(CODE)		CODE
#else
#	define HK_ON_DEBUG(CODE)
#endif

// use the compiler friendly but programmer ugly version for release only
#ifdef HK_DEBUG
#	define HK_MULTILINE_MACRO_BEGIN	do {
#	define HK_MULTILINE_MACRO_END		} while(0)
#else
#		define HK_MULTILINE_MACRO_BEGIN	if(1) {
#		define HK_MULTILINE_MACRO_END		} else
#endif

#if defined(HK_ARCH_IA32) || defined(HK_ARCH_X64)
#		define HK_BREAKPOINT(ID) __debugbreak();
#elif defined(HK_PLATFORM_LRB)
#	include <unistd.h>
#	include <signal.h>
#	define HK_BREAKPOINT(ID) signal(SIGSTOP, HK_NULL);
#elif defined(HK_PLATFORM_PS3_PPU) 
#		define HK_BREAKPOINT(ID) __asm__ volatile ( "tw 31,1,1" )
#elif defined(HK_PLATFORM_PS3_SPU)
#	define HK_BREAKPOINT(ID) __asm__ volatile ("ilhu $0, %0\n\tiohl $0, %1\n\tstopd $0,$0,$0" : : "i"(ID>>16), "i"(ID&0xFFFF) ); 
#elif defined(HK_PLATFORM_XBOX360)
#	define HK_BREAKPOINT(ID) __debugbreak();
#elif defined(HK_PLATFORM_CAFE)
#	define HK_BREAKPOINT(ID) ((*((int*)0)) = ID);
#elif defined(HK_PLATFORM_GC)
#	define HK_BREAKPOINT(ID)    HK_MULTILINE_MACRO_BEGIN						\
								{                                               \
    								register unsigned int HK_ORIGINAL_MSR;		\
    								register unsigned int HK_NEW_MSR;			\
    								asm volatile								\
									{											\
    									mfmsr HK_ORIGINAL_MSR; 					/* Grab the original value of the MSR */ \
    									ori HK_NEW_MSR, HK_ORIGINAL_MSR, 0x400; /* or the original MSR with 0x400 and store it */ \
    									mtmsr HK_NEW_MSR; 						/* Set the MSR to the new value - this will throw exception 0xD00 */ \
    									mtmsr HK_ORIGINAL_MSR; 					/* This puts the MSR back to its original state; required for*/\
										    								    /* getting out of single-step mode (when the user resumes) */\
    								}											\
                                }   											\
								HK_MULTILINE_MACRO_END

#elif defined(HK_PLATFORM_CTR)
#	include <nn/dbg.h>
#	define HK_BREAKPOINT(ID) ::nn::dbg::Break(nn::dbg::BREAK_REASON_ASSERT);

#elif defined(HK_ARCH_ARM)
#		define HK_BREAKPOINT(ID) asm("bkpt 0"); 
#else
#	define HK_BREAKPOINT(ID) ((*((int*)0)) = ID);
#endif

#define HK_NULL 0


/// Note that ALIGNMENT must be a power of two for this to work.
/// Note: to use this macro you must cast your pointer to a byte pointer or to an integer value.
#define HK_NEXT_MULTIPLE_OF(ALIGNMENT, VALUE)  ( ((VALUE) + ((ALIGNMENT)-1)) & (~((ALIGNMENT)-1)) )

/// The offset of a member within a structure
#define HK_OFFSET_OF(CLASS,MEMBER) int(reinterpret_cast<hkLong>(&(reinterpret_cast<CLASS*>(16)->MEMBER))-16)

/// A check for whether the offset of a member within a structure is as expected
#define HK_OFFSET_EQUALS(CLASS,MEMBER,OFFSET) (HK_OFFSET_OF(CLASS,MEMBER)==OFFSET)

/// Join two preprocessor tokens, even when a token is itself a macro.
#define HK_PREPROCESSOR_JOIN_TOKEN(A,B) HK_PREPROCESSOR_JOIN_TOKEN2(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN2(A,B) HK_PREPROCESSOR_JOIN_TOKEN3(A,B)
#define HK_PREPROCESSOR_JOIN_TOKEN3(A,B) A##B

/// Creates an uninitialized buffer large enough for object of type TYPE to fit in while aligned to ALIGN boundary. Creates a pointer VAR to this aligned address.
#define HK_DECLARE_ALIGNED_LOCAL_PTR( TYPE, VAR, ALIGN  )											\
	const int VAR ## BufferSize = ALIGN + sizeof(TYPE);												\
	char VAR ## Buffer[VAR ## BufferSize];															\
	TYPE* VAR = reinterpret_cast<TYPE*>( HK_NEXT_MULTIPLE_OF(ALIGN, hkUlong( VAR ## Buffer )) );	\


//
// compiler specific settings
//


	// *************************************
	//			GCC and SN
	// *************************************
#if defined(HK_COMPILER_GCC) || defined(HK_COMPILER_SNC) || defined(HK_COMPILER_GHS) || defined(__GCCXML__)
#       if defined(__GNUC_PATCHLEVEL__)
#   	    define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 +__GNUC_PATCHLEVEL__)
#	    else
#		    define HK_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#		endif
#	define HK_RESTRICT __restrict
#	if (HK_COMPILER_GCC_VERSION >= 40000)
#		undef HK_OFFSET_OF
#		define HK_OFFSET_OF(CLASS,MEMBER) __builtin_offsetof(CLASS,MEMBER)
#	elif ( HK_COMPILER_GCC_VERSION >= 30400 ) && !defined( HK_COMPILER_SNC )
#		undef HK_OFFSET_OF
#		define HK_OFFSET_OF(CLASS,MEMBER) (__offsetof__(reinterpret_cast<long>(&reinterpret_cast<CLASS*>(16)->MEMBER)-16))
#	endif
#	if ( HK_COMPILER_GCC_VERSION >= 40000 ) && !defined( HK_COMPILER_SNC )
#		undef HK_OFFSET_EQUALS
#		define HK_OFFSET_EQUALS(C,M,O) true
#	endif
#	define HK_ALIGN(DECL, ALIGNMENT) DECL __attribute__((aligned(ALIGNMENT)))
#	define HK_ALIGN8(DECL) DECL __attribute__((aligned(8)))
#   define HK_ALIGN16(DECL) DECL __attribute__((aligned(16)))
#   define HK_ALIGN32(DECL) DECL __attribute__((aligned(32)))
#   define HK_ALIGN64(DECL) DECL __attribute__((aligned(64)))
#   define HK_ALIGN128(DECL) DECL __attribute__((aligned(128)))
#	define HK_CLASSALIGN(DECL, ALIGNMENT) HK_ALIGN(DECL, ALIGNMENT)
#	define HK_CLASSALIGN8(DECL) HK_ALIGN8(DECL)
#	define HK_CLASSALIGN16(DECL) HK_ALIGN16(DECL)
#	if defined(HK_ARCH_ARM) || defined(HK_COMPILER_GHS)
#		define HK_ALIGN_RELAX_CHECKS 1
#	endif
#		define HK_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))

#	if defined(HK_COMPILER_GHS) 
#		define HK_LINKONCE_VTABLES 0
#	endif

	    typedef unsigned long long hkUint64;
	    typedef long long hkInt64;
#if defined(HK_PLATFORM_CTR)
		typedef unsigned int hkSystemTime;
#	else
		typedef long hkSystemTime;
#	endif
#	if defined(HK_PLATFORM_MAC386) || defined(HK_PLATFORM_MACPPC) || defined(HK_PLATFORM_IOS) || ( defined(HK_PLATFORM_PS3_PPU) && ( HK_CELL_SDK_VERSION < 0x080000 ) ) || defined (HK_PLATFORM_PS3_SPU)
		typedef unsigned long hk_size_t;
#	elif (defined(HK_PLATFORM_LINUX) && defined(HK_ARCH_X64)) || (defined(__GCCXML__) && defined(__x86_64)) // LINUX64
		typedef long unsigned int hk_size_t;
#	else
		typedef unsigned hk_size_t;
#	endif
#	if defined(HK_ARCH_IA32) && HK_COMPILER_GCC_VERSION >= 30100
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	endif
#ifdef __HAVOK_PARSER__
#   define HK_FORCE_INLINE /*nothing*/
#elif defined(HK_PLATFORM_CTR)
#	define HK_FORCE_INLINE __forceinline
#else
#	define HK_FORCE_INLINE inline
#endif
// calling convention
#	define HK_CALL
#	define HK_FAST_CALL
// deprecation
#	define HK_DEPRECATED /* nothing */
#	define HK_DEPRECATED2(MSG) /* nothing */
// Section attribute. Placing each function in a different section allows the linker to dead-strip them individually
#if !defined(HK_PLATFORM_IOS) && !defined(HK_PLATFORM_MAC386) && !defined(HK_PLATFORM_NGP) && !defined(HK_PLATFORM_CAFE)
#	define HK_INIT_FUNCTION2(FN, SECTION_SUFFIX)	__attribute__ ((section (".text_init." #SECTION_SUFFIX))) FN
#	define HK_INIT_FUNCTION(FN)						HK_INIT_FUNCTION2(FN, FN)
#endif

	// *************************************
	//			MICROSOFT and INTEL
	// *************************************
#elif defined(HK_COMPILER_MSVC) || defined(HK_COMPILER_INTEL)
#	define HK_COMPILER_SUPPORTS_PCH
#	define HK_COMPILER_MSVC_VERSION _MSC_VER
#	ifdef __INTEL_COMPILER
#		define HK_COMPILER_INTEL_VERSION __INTEL_COMPILER
#	else
#		define HK_COMPILER_INTEL_VERSION _MSC_VER
#	endif
#	if (_MSC_VER >= 1400) // 2005 only
#		define HK_RESTRICT __restrict
#	else
#		define HK_RESTRICT 
#	endif
#	pragma warning( disable : 4786 ) // Debug tuncated to 255:
#	pragma warning( disable : 4530 ) // C++ Exception handler used but not enabled:(used in <xstring>)
#		define HK_ALIGN(DECL, ALIGNMENT) __declspec(align(ALIGNMENT)) DECL
#		define HK_ALIGN8(DECL) __declspec(align(8)) DECL
#		define HK_ALIGN16(DECL) __declspec(align(16)) DECL
#		define HK_ALIGN32(DECL) __declspec(align(32)) DECL
#		define HK_ALIGN64(DECL) __declspec(align(64)) DECL
#		define HK_ALIGN128(DECL) __declspec(align(128)) DECL
#		if !defined(HK_COMPILER_INTEL)
#			define HK_FORCE_INLINE __forceinline
#		else // ICC has no force inline intrinsic
#			define HK_FORCE_INLINE inline
#		endif
#		define HK_CLASSALIGN(DECL, ALIGNMENT) HK_ALIGN(DECL, ALIGNMENT)
#		define HK_CLASSALIGN8(DECL) HK_ALIGN8(DECL)
#		define HK_CLASSALIGN16(DECL) HK_ALIGN16(DECL)
#		define HK_VISIBILITY_HIDDEN /*empty*/
#		define HK_NEVER_INLINE __declspec(noinline)
#	if defined(HK_ARCH_IA32)
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef long hkSystemTime;
#		if defined(HK_COMPILER_MSVC) && (_MSC_VER >= 1300)
			typedef unsigned __w64 hk_size_t; // VC7.0 or higher, 64bit warnings
#		else
			typedef unsigned hk_size_t;
#		endif
#		define HK_COMPILER_HAS_INTRINSICS_IA32
#	elif defined( HK_ARCH_PPC )
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef unsigned hk_size_t;
		typedef __int64 hkSystemTime; // 64bit time
#		define HK_COMPILER_HAS_INTRINSICS_PPC
#	elif defined(HK_ARCH_X64) || defined(HK_ARCH_LRB)
		typedef unsigned __int64 hkUint64;
		typedef __int64 hkInt64;
		typedef __int64 hkSystemTime;
		typedef unsigned __int64 hk_size_t;
#		ifdef HK_ARCH_LRB
#			define HK_COMPILER_HAS_INTRINSICS_LRB
#		else
#			define HK_COMPILER_HAS_INTRINSICS_IA32
#		endif
#	else
#		error No defs for this architecture
#	endif
// calling convention
#	ifndef HK_COMPILER_INTEL
#		define HK_CALL __cdecl
#		define HK_FAST_CALL __fastcall
#	else
#		define HK_CALL 
#		define HK_FAST_CALL 
#	endif
// deprecation
#	if defined(HK_PLATFORM_WIN32) && (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#		define HK_DEPRECATED __declspec(deprecated)
#		define HK_DEPRECATED2(MSG) __declspec(deprecated(MSG))
#	else
#		define HK_DEPRECATED /* nothing */
#		define HK_DEPRECATED2(MSG) /* nothing */
#	endif

	// *************************************
	//			METROWERKS
	// *************************************
#elif defined(HK_COMPILER_MWERKS)
#	define HK_ALIGN(DECL, ALIGNMENT) DECL __attribute__((aligned(ALIGNMENT)))
#	define HK_ALIGN16(DECL) DECL __attribute__((aligned(16)))
#	define HK_ALIGN32(DECL) DECL __attribute__((aligned(32)))
#	define HK_ALIGN64(DECL) DECL __attribute__((aligned(64)))
#	define HK_ALIGN128(DECL) DECL __attribute__((aligned(128)))
#	define HK_CLASSALIGN(DECL, ALIGNMENT) DECL
#	define HK_CLASSALIGN16(DECL) DECL
#	define HK_VISIBILITY_HIDDEN /*empty*/
		typedef unsigned long long hkUint64;
		typedef long long hkInt64;
			typedef unsigned long hkSystemTime;
		typedef unsigned long hk_size_t;
#	define HK_FORCE_INLINE inline
// calling convention
#	define HK_CALL
#	define HK_FAST_CALL
// deprecation
#	define HK_DEPRECATED /* nothing */
#	define HK_DEPRECATED2(MSG) /* nothing */

#else
#	error Unknown compiler
#endif // compilers

#ifndef HK_LINKONCE_VTABLES
#define HK_LINKONCE_VTABLES 1 
#endif

#ifndef HK_NATIVE_ALIGNMENT
#define HK_NATIVE_ALIGNMENT 16
#endif


#	define HK_ON_PLATFORM_HAS_SPU(code)

#	define HK_PLATFORM_MULTI_THREAD

#	define HK_ALWAYS_INLINE HK_FORCE_INLINE
#	define HK_LOCAL_INLINE HK_FORCE_INLINE
#	define HK_ASM_SEP(a) 

#ifndef HK_NEVER_INLINE
#		define HK_NEVER_INLINE
#endif

#ifndef HK_AUTO_INLINE
#		define HK_AUTO_INLINE HK_FORCE_INLINE
#endif

#	define HK_NOSPU_VIRTUAL virtual
#	define HK_CPU_CONST const

#ifndef HK_RESTRICT
#	define HK_RESTRICT
#endif
#ifndef HK_VERY_UNLIKELY
#	define HK_VERY_UNLIKELY(EXPR) EXPR
#	define HK_VERY_LIKELY(EXPR) EXPR
#endif
#ifndef HK_INIT_FUNCTION
#	define HK_INIT_FUNCTION( FN ) FN
#	define HK_INIT_FUNCTION2(FN, SECTION_SUFFIX) FN
#endif

typedef hkUint16 hkObjectIndex;
typedef hkReal hkTime;

#define HK_INVALID_OBJECT_INDEX 0xffff


HK_FORCE_INLINE hkInt32 HK_CALL hkPointerToInt32( const void* ptr )
{
	return static_cast<int>( hkUlong(ptr) );
}

/// get the byte offset of B - A, as a full long.
HK_FORCE_INLINE hkLong HK_CALL hkGetByteOffset( const void* base, const void* pntr)
{
	return hkLong(pntr) - hkLong(base);
}

/// get the byte offset of B - A, as an int (64bit issues, so here for easy code checks)
HK_FORCE_INLINE int HK_CALL hkGetByteOffsetInt( const void* base, const void* pntr)
{
	return static_cast<int>( hkGetByteOffset( base, pntr ) );
}

/// get the byte offset of B - A, as a full 64bit hkUint64.
HK_FORCE_INLINE hkInt32 HK_CALL hkGetByteOffsetCpuPtr( const HK_CPU_PTR(void*) base, const HK_CPU_PTR(void*) pntr)
{
	return hkInt32(hkLong((HK_CPU_PTR(const char*))(pntr) - (HK_CPU_PTR(const char*))(base)));
}

template <typename TYPE>
HK_ALWAYS_INLINE TYPE* HK_CALL hkAddByteOffset( TYPE* base, hkLong offset )
{
	return reinterpret_cast<TYPE*>( reinterpret_cast<char*>(base) + offset );
}

template <typename TYPE>
HK_ALWAYS_INLINE TYPE HK_CALL hkAddByteOffsetCpuPtr( TYPE base, hkLong offset )
{
	return reinterpret_cast<TYPE>( reinterpret_cast<char*>(base) + offset );
}

template <typename TYPE>
HK_ALWAYS_INLINE const TYPE* HK_CALL hkAddByteOffsetConst( const TYPE* base, hkLong offset )
{
	return reinterpret_cast<const TYPE*>( reinterpret_cast<const char*>(base) + offset );
}

template <typename TYPE>
HK_ALWAYS_INLINE TYPE HK_CALL hkAddByteOffsetCpuPtrConst( TYPE base, hkLong offset )
{
	return reinterpret_cast<TYPE>( reinterpret_cast<const char*>(base) + offset );
}

	/// If you have a pair of pointers and you have one pointer, than this function allows you to quickly get the other pointer of the pair.
template <typename TYPE>
HK_ALWAYS_INLINE TYPE HK_CALL hkSelectOther( TYPE a, TYPE pairA, TYPE pairB )
{
	return (TYPE)( hkUlong(a) ^ hkUlong(pairA) ^ hkUlong(pairB) );
}

/// If you have a pair of pointers and you have one pointer, than this function allows you to quickly get the other pointer of the pair.
template <typename TYPE>
HK_ALWAYS_INLINE TYPE* HK_CALL hkSelect( int select, TYPE* pairA, TYPE* pairB )
{
	//HK_ASSERT( 0xf0345456, select == 0 || select == 1);
	hkUlong ua = hkUlong(pairA);
	hkUlong ub = hkUlong(pairB);
	return reinterpret_cast<TYPE*>( ua ^ ((ua^ub)&(-select)) );
}




HK_FORCE_INLINE hkUint32 hkNextPowerOf2(hkUint32 in) 
{
	in -= 1;

	in |= in >> 16;
	in |= in >> 8;
	in |= in >> 4;
	in |= in >> 2;
	in |= in >> 1;

	return in + 1; 
}

HK_FORCE_INLINE hkUint32 hkBitmask(int width)
{
	// return (1<<width) - 1,
	// except it must work for width = 32
	// (bear in mind that shift by >=32 places is undefined)
	hkUint32 maskIfNot32 = (1 << (width & 31)) - 1; // ans if width != 32, else 0
	hkUint32 is32 = ((width & 32) >> 5); // 1 if width = 32, else 0
	return maskIfNot32 - is32; // the answer in the case width=32 is -1
}

class hkFinishLoadedObjectFlag
{
	//+hk.MemoryTracker(ignore=True)
public:
	hkFinishLoadedObjectFlag() : m_finishing(0) {}
	int m_finishing;
};

#define hkSizeOf(A) int(sizeof(A))

#define HK_REFLECTION_CLASSFILE_DESTINATION(PATH)
#define HK_REFLECTION_CLASSFILE_HEADER(PATH)
#define HK_DECLARE_REFLECTION() \
	static const struct hkInternalClassMember Members[]; \
	static const hkClass& HK_CALL staticClass(); \
	struct DefaultStruct

class hkClass;

/// A generic object with metadata.

struct hkVariant
{
	//+hk.MemoryTracker(ignore=True)
	void* m_object;
	const hkClass* m_class;
};




/// A floating point value stored in 16-bit. This is a non IEEE representation using
/// 1 sign bit, 8 exponent bits and 7 mantissa bits. This is basically a 32-bit float
/// truncated. It supports the same floating point range as 32-bit float,
/// but only with 6 bit precision (because the last bit is not rounded and renormalized).
class hkHalf
{
	//+hk.MemoryTracker(ignore=True)
public:
	HK_FORCE_INLINE hkHalf() { }

	HK_FORCE_INLINE explicit hkHalf(const float& f)
	{
		int t = ((const int*)&f)[0];
		m_value = hkInt16(t>>16);
	}

	HK_FORCE_INLINE hkHalf& operator=(const float& f)
	{
		int t = ((const int*)&f)[0];
		m_value = hkInt16(t>>16);
		return *this;
	}

	HK_FORCE_INLINE hkHalf& operator=(const hkHalf& other)
	{
		m_value = other.m_value;
		return *this;
	}

	HK_FORCE_INLINE bool operator==(const hkHalf& other) const
	{
		return (m_value == other.m_value);
	}

	HK_FORCE_INLINE void store( float* f ) const
	{
		hkUint32* asInt = (hkUint32*)f;
		asInt[0] = m_value << 16;
	}

	HK_FORCE_INLINE void setZero()
	{
		m_value = 0;
	} 

	HK_FORCE_INLINE void setOne()
	{
		m_value = 0x3f80;
	}

	HK_FORCE_INLINE operator float() const
	{
		union
		{
			int i;
			float f;
		} u;
		u.i = (m_value <<16);
		return u.f;
	}

	HK_FORCE_INLINE hkInt16 getInt16() const
	{
		return m_value;
	}

private:
	hkInt16 m_value;
};

/// A floating point value stored in 16-bit according to the IEEE 754-2008
/// format 'binary16' which has 1 sign bit, 5 exponent bits and 10 mantissa bits.
/// This format is compatible with the widely used GPU 'half' variable format.
/// The supported value range is 6.1035156e-5 to 131008.0 (both pos/neg) with
/// 10 bit precision.
///
/// \warning For performance reasons, our implementation differs from the 
///          IEEE and some GPU definitions in that 32-bit values greater than 
///          the max half value do not get mapped to infinity but to max.
class hkFloat16
{
public:

	//+hk.MemoryTracker(ignore=True)
	HK_DECLARE_REFLECTION();

public:

	HK_FORCE_INLINE hkFloat16& operator=(const hkFloat16& other)
	{
		m_value = other.m_value;
		return *this;
	}

	HK_FORCE_INLINE void setZero()
	{
		m_value = 0x0000;
	} 

	HK_FORCE_INLINE void setOne()
	{
		m_value = 0x3c00;
	}

	template <bool ROUND>
	HK_FORCE_INLINE void setReal(hkReal r);

	HK_FORCE_INLINE hkReal getReal() const
	{
		enum {
			sign_pos = 15,
			half_sign = 1 << sign_pos,
			half_exponent = 0x00007C00,
			mantissa_width   = 23,
			mantissa_width_h = 10,
			mantissa_mask = (1 << mantissa_width_h) - 1,
			adjust = mantissa_width - mantissa_width_h,
			half_bias_offset = 0x0001C000
		};

		const hkUint32
			sign = m_value & half_sign,
			exponent = m_value & half_exponent,
			mantissa = m_value & mantissa_mask,
			exp_offset = exponent + half_bias_offset,
			bits = (sign << (sign_pos+1)) | ((exp_offset | mantissa) << adjust);

		const hkFloat32* bits_f = (const hkFloat32*)&bits;
		return exponent ? hkReal(*bits_f) : hkReal(0);
	}

	HK_FORCE_INLINE hkUint16 getBits() const
	{
		return m_value;
	}

	HK_FORCE_INLINE void setBits(hkUint16 b)
	{
		m_value = b;
	}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
	HK_FORCE_INLINE hkFloat16() { }
	HK_FORCE_INLINE explicit hkFloat16(const hkFloat32& f);
#endif

private:
	hkUint16 m_value;
};


template <>
HK_FORCE_INLINE void hkFloat16::setReal<true>(hkReal r)
{
	const hkUint32 infinity    = 0x47FFE000; // Largest positive half
	const hkUint32 halfNormMin = 0x38800000; // Smallest positive normalized half

	enum {
		mantissa_width   = 23,
		mantissa_width_h = 10,
		mantissa_mask = (1<<mantissa_width) - 1,
		sign_mask = 0x80000000,
		adjust = mantissa_width - mantissa_width_h,
		exponent_fix = hkUint8(-(127-15)) << mantissa_width
	};

	const hkFloat32 bits_f = hkFloat32(r);
	const hkUint32
		bits_i = (bits_f > *((const hkFloat32*)&infinity)) ? infinity : *((const hkUint32*)&bits_f),
		exp = bits_i + exponent_fix,
		round = (bits_i + 0xFFFu) + ((bits_i >> adjust) & 1u),
		result = (exp & ~mantissa_mask) | (round & mantissa_mask),
		h = (result >> adjust) | ((bits_i & sign_mask) >> 16),
		absbits = bits_i & 0x7FFFFFFF;
	m_value = (absbits >= halfNormMin) ? (hkUint16)h : 0;
}

template <bool ROUND>
HK_FORCE_INLINE void hkFloat16::setReal(hkReal r)
{
	const hkUint32 infinity    = 0x47FFE000; // Largest positive half
	const hkUint32 halfNormMin = 0x38800000; // Smallest positive normalized half

	enum {
		mantissa_width   = 23,
		mantissa_width_h = 10,
		mantissa_mask = (1<<mantissa_width) - 1,
		sign_mask = 0x80000000,
		adjust = mantissa_width - mantissa_width_h,
		exponent_fix = hkUint8(-(127-15)) << mantissa_width
	};

	const hkFloat32 bits_f = hkFloat32(r);
	const hkUint32
		bits_i = (bits_f > *((const hkFloat32*)&infinity)) ? infinity : *((const hkUint32*)&bits_f),
		exp = bits_i + exponent_fix,
		result = (exp & ~mantissa_mask) | (bits_i & mantissa_mask),
		h = (result >> adjust) | ((bits_i & sign_mask) >> 16),
 		absbits = bits_i & 0x7FFFFFFF;
	m_value = (absbits >= halfNormMin) ? (hkUint16)h : 0;
}

#ifndef HK_DISABLE_MATH_CONSTRUCTORS
HK_FORCE_INLINE hkFloat16::hkFloat16(const hkFloat32& f)
{
	setReal<true>(f);
}
#endif

//#define HK_BOOL32_IS_STRICT
enum hkBool32FalseType { hkFalse32 = 0 };

#if defined(HK_BOOL32_IS_STRICT)
class hkBool32;
#else
#		pragma warning( disable : 4800 ) // forcing value to bool 'true' or 'false' (performance warning)
	/// Bool used for inner loops.
	///
	/// False is zero, true is _any_ non-zero value. Advice: simply check for != false.
	/// \warning The following operations on hkBool32 will not work as expected: ==, >, <, ~, ^, &
	/// These will work: ! , &&, ||, |
typedef hkUint32 hkBool32;
#endif

	/// A wrapper to store a hkBool in one byte, regardless of compiler options.
class hkBool
{
	//+hk.MemoryTracker(ignore=True)
	public:

			// used in compile time asserts
		typedef char CompileTimeTrueType;
		typedef int CompileTimeFalseType;

		HK_ALWAYS_INLINE hkBool()
		{
		}

		HK_FORCE_INLINE hkBool(bool b)
		{
			m_bool = static_cast<char>(b);
		}


#if defined(HK_BOOL32_IS_STRICT)
		HK_FORCE_INLINE hkBool(int  b){ m_bool = b!=0; }
		HK_FORCE_INLINE hkBool(hkUint32  b){ m_bool = b!=0; }
		HK_FORCE_INLINE hkBool(const hkBool32& b);
		HK_FORCE_INLINE hkBool& operator=(const hkBool32& e);
#endif

		HK_FORCE_INLINE operator bool() const
		{
			return m_bool != 0;
		}

		HK_FORCE_INLINE hkBool& operator=(bool e)
		{
			m_bool = static_cast<char>(e);
			return *this;
		}


		HK_FORCE_INLINE hkBool operator==(bool e) const
		{
			return (m_bool != 0) == e;
		}

		HK_FORCE_INLINE hkBool operator!=(bool e) const
		{
			return (m_bool != 0) != e;
		}

	private:
#if defined(HK_BOOL32_IS_STRICT)
		friend class hkBool32;
#endif
		char m_bool;
};


#if defined(HK_BOOL32_IS_STRICT)
class hkBool32
{
	// operators that work as expected
public:

	HK_FORCE_INLINE hkBool32() {}

	HK_FORCE_INLINE hkBool32(const hkBool& b)  { m_bool = b.m_bool; }

	HK_FORCE_INLINE hkBool32(hkUint32 i) : m_bool(i) {}

	HK_FORCE_INLINE hkBool32(const hkBool32& rhs): m_bool(rhs.m_bool) {}

	HK_FORCE_INLINE operator bool() const { return m_bool != 0; }

	HK_FORCE_INLINE operator hkBool() const { return hkBool(m_bool != 0); }

	//HK_FORCE_INLINE hkBool32& operator =(const hkBool32& b) { m_bool = b.m_bool; return *this; }

	HK_FORCE_INLINE bool operator !() const { return !m_bool; }

	HK_FORCE_INLINE bool operator &&(const hkBool32& b) const { return m_bool && b.m_bool; }
	HK_FORCE_INLINE bool operator &&(const hkBool& b) const { return m_bool && b; }
	HK_FORCE_INLINE bool operator &&(const bool b) const { return (m_bool!=0) && b; }

	HK_FORCE_INLINE bool operator ||(const hkBool32& b) const { return m_bool || b.m_bool; }
	HK_FORCE_INLINE bool operator ||(const hkBool& b) const { return m_bool || b; }
	HK_FORCE_INLINE bool operator ||(const bool b) const { return (m_bool!=0) || b; }

	//	HK_FORCE_INLINE	bool operator !=(const bool b) const{ if ( b!= false) {HK_BREAKPOINT(0xf0345456);}; return m_bool != 0; }
	//	HK_FORCE_INLINE	bool operator ==(const bool b) const{ if ( b!= false) {HK_BREAKPOINT(0xf0345456);}; return m_bool == 0; }
	//	HK_FORCE_INLINE	bool operator !=(const int b) const{ if ( b!= 0) {HK_BREAKPOINT(0xf0345456);}; return m_bool != 0; }
	HK_FORCE_INLINE	bool operator ==(hkBool32FalseType) const{ return m_bool == 0; }
	HK_FORCE_INLINE	bool operator !=(hkBool32FalseType) const{ return m_bool != 0; }

	HK_FORCE_INLINE hkBool32 operator |(const hkBool32& b) const { return m_bool | b.m_bool; }

	HK_FORCE_INLINE hkBool32& operator |=(const hkBool32& b) { m_bool |= b.m_bool; return *this; }

	// operators that don't work
private:

	bool operator ==(const hkBool32& b) const;
	bool operator !=(const hkBool32& b) const;
	bool operator <=(const hkBool32& b) const;
	bool operator >=(const hkBool32& b) const;
	bool operator < (const hkBool32& b) const;
	bool operator > (const hkBool32& b) const;

	hkBool32 operator ~() const;
	hkBool32 operator &(const hkBool32& b) const;
	hkBool32 operator ^(const hkBool32& b) const;

	hkBool32& operator &=(const hkBool32& b);
	hkBool32& operator ^=(const hkBool32& b);

public:

	hkUint32 m_bool;
};
#endif

#if defined(HK_BOOL32_IS_STRICT)
	HK_FORCE_INLINE hkBool::hkBool(const hkBool32& b)
	{
		m_bool = bool(b);
	}

	HK_FORCE_INLINE hkBool& hkBool::operator=(const hkBool32& b)
	{
		m_bool = bool(b);
		return *this;
	}
#endif


#define HK_UFLOAT8_MAX_VALUE 256

	/// A wrapper to store an unsigned float into 8 bit.
	/// This has a reduced range. Basically the encoding
	/// uses a table holding an exponential function.
	/// The range is [0.010f to 1000002.f] with an average error of 7%
class hkUFloat8
{
	//+hk.MemoryTracker(ignore=True)
public:
	enum
	{
		MAX_VALUE = HK_UFLOAT8_MAX_VALUE,
	
		// Constants used in compressing / decompressing the values table
		ENCODED_EXPONENT_BITS	= 5,
		ENCODED_MANTISSA_BITS	= (16 - ENCODED_EXPONENT_BITS),
		ENCODED_EXPONENT_BIAS	= 119,
		ENCODED_EXPONENT_MASK	= (1 << ENCODED_EXPONENT_BITS) - 1,
		ENCODED_MANTISSA_MASK	= (1 << ENCODED_MANTISSA_BITS) - 1,

		FLOAT_MANTISSA_BITS		= 23,
		FLOAT_MANTISSA_MASK		= (1 << FLOAT_MANTISSA_BITS) - 1,

		ENCODE_MANTISSA_SHIFT	= (FLOAT_MANTISSA_BITS - ENCODED_MANTISSA_BITS),
		DECODE_EXPONENT_BIAS	= (ENCODED_EXPONENT_BIAS << (FLOAT_MANTISSA_BITS - ENCODE_MANTISSA_SHIFT)),
	};

		// the minimum value to encode which is non zero
#define hkUFloat8_eps 0.01f

		// the maximum value to encode
#define hkUFloat8_maxValue 1000000.0f

		HK_FORCE_INLINE hkUFloat8(){		}

		hkUFloat8& operator=(const float& fv);

		HK_FORCE_INLINE bool isZero() const { return m_value == 0; }
		HK_FORCE_INLINE void setZero(){ m_value = 0; }

		HK_FORCE_INLINE bool isMax() const { return m_value == 255; }
		HK_FORCE_INLINE void setMax(){ m_value = 255; }



		HK_FORCE_INLINE hkBool operator==( const hkUFloat8& other ) const 
		{
			return m_value == other.m_value;
		}
		
		HK_FORCE_INLINE hkUFloat8(const float f)
		{
			*this = f;
		}

			/// this = sqrt(a*b)
		HK_FORCE_INLINE void setArithmeticMean( const hkUFloat8& a, const hkUFloat8& b)
		{
			m_value = hkUint8( (int(a.m_value) + int(b.m_value) )>>1 );
		}


		HK_FORCE_INLINE operator float() const
		{
			return decodeFloat(getEncodedFloat(m_value));
		}

		static HK_FORCE_INLINE float HK_CALL decodeFloat(const hkUint16 i)
		{
			const int intExpo = (i + hkUFloat8::DECODE_EXPONENT_BIAS) << hkUFloat8::ENCODE_MANTISSA_SHIFT;
			union { const float* f; const int* i; } f2i;	f2i.i = &intExpo;
			return i ? *f2i.f : 0.0f;
		}

	public:

		hkUint8 m_value;

	protected:

		/// Function used to access the table of encoded float values. Required to make sure that the 
		/// table is initialized before any construction occurs
		static HK_FORCE_INLINE hkUint16 HK_CALL getEncodedFloat(hkUint8 index);
};

// A lookup table for converting unsigned char to float
// useful for avoiding LHS

extern "C"
{
	extern const hkReal hkUInt8ToReal[256];
}

	/// A wrapper to store an enum with explicit size.
template<typename ENUM, typename STORAGE>
class hkEnum
{
	public:

		HK_ALWAYS_INLINE hkEnum()
		{
		}

		hkEnum(ENUM e)
		{
			m_storage = static_cast<STORAGE>(e);
		}

		operator ENUM() const
		{
			return static_cast<ENUM>(m_storage);
		}
		void operator=(ENUM e)
		{
			m_storage = static_cast<STORAGE>(e);
		}
		hkBool operator==(ENUM e) const
		{
			return m_storage == static_cast<STORAGE>(e);
		}
		hkBool operator!=(ENUM e) const
		{
			return m_storage != static_cast<STORAGE>(e);
		}

	private:

		STORAGE m_storage;
};

	/// A wrapper to store bitfield with an with explicit size.
template<typename BITS, typename STORAGE>
class hkFlags
{
	public:

		hkFlags()
		{
		}

		hkFlags(STORAGE s)
		{
			m_storage = s;
		}

		void clear()
		{
			m_storage = 0;
		}
		void setAll( STORAGE s )
		{
			m_storage = s;
		}

		void orWith( STORAGE s )
		{
			m_storage |= s;
		}

		void xorWith( STORAGE s )
		{
			m_storage ^= s;
		}

		void andWith( STORAGE s )
		{
			m_storage &= s;
		}

		void setWithMask( STORAGE s, STORAGE mask )
		{
			m_storage = (m_storage & ~mask) | (s & mask);
		}

		STORAGE get() const
		{
			return m_storage;
		}

		STORAGE get( STORAGE mask ) const
		{
			return m_storage & mask;
		}

		hkBool32 anyIsSet( STORAGE mask ) const
		{
			return (m_storage & mask);
		}
		
		bool allAreSet( STORAGE mask ) const
		{
			return (m_storage & mask) == mask;
		}

		bool operator==( const hkFlags& f ) const
		{
			return f.m_storage == m_storage;
		}

		hkBool32 operator!=( const hkFlags& f ) const
		{
			return f.m_storage ^ m_storage;
		}

	private:

		STORAGE m_storage;
};

#	define HK_PADSPU_PROMOTE(e) e
#	define HK_PADSPU_EXTRACT(e) e

	/// wrapper class for variables in structures.
	/// Basically on the PlayStation(R)3 SPU, the SPU can only poorly
	/// access non aligned members. This class give each variable
	/// 16 bytes, thereby dramatically decreasing code size and CPU overhead
template <typename TYPE>
class hkPadSpu
{
	public:

		HK_ALWAYS_INLINE hkPadSpu() {}

		HK_ALWAYS_INLINE hkPadSpu(TYPE e)
			: m_storage( HK_PADSPU_PROMOTE(e) )
		{
		}

		HK_ALWAYS_INLINE void operator=(TYPE e)
		{
			m_storage = HK_PADSPU_PROMOTE(e);
		}

		HK_ALWAYS_INLINE TYPE val() const
		{
			return HK_PADSPU_EXTRACT(m_storage);
		}

		HK_ALWAYS_INLINE TYPE& ref() const
		{
			return *(TYPE*)&m_storage;
		}

		HK_ALWAYS_INLINE TYPE operator->() const
		{
			return HK_PADSPU_EXTRACT(m_storage);
		}

		HK_ALWAYS_INLINE operator TYPE() const
		{
			return HK_PADSPU_EXTRACT(m_storage);
		}

		HK_ALWAYS_INLINE void operator+=(TYPE e)
		{
			*this = val() + e;
		}

		HK_ALWAYS_INLINE void operator-=(TYPE e)
		{
			*this = val() - e;
		}

		HK_ALWAYS_INLINE hkPadSpu& operator++()
		{
			operator+=(1);
			return *this;
		}

		HK_ALWAYS_INLINE TYPE operator++(int)
		{
			const TYPE value = val();
			operator+=(1);
			return value;
		}

		HK_ALWAYS_INLINE hkPadSpu& operator--()
		{
			operator-=(1);
			return *this;
		}

		HK_FORCE_INLINE TYPE operator--(int)
		{
			const TYPE value = val();
			operator-=(1);
			return value;
		}

	private:

		TYPE m_storage;
};

#	define HK_PAD_ON_SPU(TYPE) TYPE
#	define HK_ON_CPU(code) code
#	define HK_ON_SPU(code)
#	define HK_CPU_ARG(code) , code
#	define HK_SPU_ARG(code)

#define HK_HINT_SIZE16(A) hkInt16(A)


namespace hkCompileError
{
	template <bool b> struct VAR_UNROLL_X_OUT_OF_RANGE;
	template <> struct VAR_UNROLL_X_OUT_OF_RANGE<true>{ };
}

#ifndef HK_DEBUG

#define hkVAR_UNROLL(x,c)	HK_MULTILINE_MACRO_BEGIN \
{	\
	HK_COMPILE_TIME_ASSERT2( (x) <= 4, VAR_UNROLL_X_OUT_OF_RANGE );	\
{ const int hkI = 0; if ( hkI < (x) ){c;} }	\
{ const int hkI = 1; if ( hkI < (x) ){c;} }	\
{ const int hkI = 2; if ( hkI < (x) ){c;} }	\
{ const int hkI = 3; if ( hkI < (x) ){c;} }	\
}	\
	HK_MULTILINE_MACRO_END

#else // fix warning C4789 in vc2010 debug

#define hkVAR_UNROLL(x,c)	HK_MULTILINE_MACRO_BEGIN \
{	\
	HK_COMPILE_TIME_ASSERT2( (x) <= 4, VAR_UNROLL_X_OUT_OF_RANGE );	\
{ for(int hkI = 0; hkI < (x); ++hkI) { c; } } \
}	\
	HK_MULTILINE_MACRO_END

#endif


struct hkCountOfBadArgCheck
{
	//+hk.MemoryTracker(ignore=True)
	class ArgIsNotAnArray;
	template<typename T> static ArgIsNotAnArray isArrayType(const T*, const T* const*);
	static int isArrayType(const void*, const void*);
};

	/// Returns the number of elements in the C array.
#define HK_COUNT_OF(x) ( \
	0 * sizeof( reinterpret_cast<const ::hkCountOfBadArgCheck*>(x) ) + \
	0 * sizeof( ::hkCountOfBadArgCheck::isArrayType((x), &(x)) ) + \
	sizeof(x) / sizeof((x)[0]) ) 

#	define HK_SPU_INIT_STACK_SIZE_TRACE()
#	define HK_SPU_UPDATE_STACK_SIZE_TRACE()
#	define HK_SPU_OUTPUT_STACK_SIZE_TRACE()
#	define HK_SPU_BSS_GUARD_INIT()
#	define HK_SPU_BSS_GUARD_CHECK()
#	define HK_SPU_STACK_POINTER_CHECK()

#include <Common/Base/Types/hkBaseTypes.inl>

#endif // HKBASE_HKBASETYPES_H

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
