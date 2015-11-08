/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBASE_SET_H
#define HKBASE_SET_H

template <typename T, typename Allocator=hkContainerHeapAllocator, typename Ops=hkMapOperations<T> >
class hkSet
{
public:

	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_MAP, hkSet );

	typedef hkSet<T,hkContainerTempAllocator> Temp;
	typedef hkSet<T,hkContainerDebugAllocator> Debug;
	typedef hkSet<T,hkContainerHeapAllocator> Heap;

	/// Iterator type.
	/// All iterators are invalidated after a mutating operation, i.e., insertion, removal.
	typedef class Dummy* Iterator;
#		define HK_MAP_INDEX_TO_ITERATOR(i) reinterpret_cast<Iterator>( hkUlong(i) )
#		define HK_MAP_ITERATOR_TO_INDEX(it) static_cast<int>( reinterpret_cast<hkUlong>(it) )

	/// Create an empty pointer map.
	hkSet() : m_numElems(0) {}
	hkSet( hkFinishLoadedObjectFlag f) : m_elem(f) {}

	/// Check whether or not the set contains the element
	HK_FORCE_INLINE hkBool32 contains(T elem) const { return isValid(findElement(elem)); }

	/// Adds the element to the set. Returns true if the elem is new, or false if it was already in the set.
	HK_FORCE_INLINE hkBool32 insert( T elem );

	///	Try to insert element. Elements are unique and cannot be -1.
	/// Returns true if the elem is new, or false if it was already in the set.
	/// If insertion fails (due to being unable to expand the table), res is set to HK_FAILURE,
	/// otherwise it is set to HK_SUCCESS.
	HK_FORCE_INLINE hkBool32 tryInsert( T elem, hkResult& res );

	/// Remove element at it. The iterator is no longer valid after this call.
	void remove( Iterator it );

	/// If element present, remove it and return HK_SUCCESS. Otherwise return HK_FAILURE.
	hkResult remove( T elem );

	/// Return the number of elements in this set.
	int getSize() const {return m_numElems; }

	/// Return the current capacity of the set.
	int getCapacity() const { return (getHashMod() + 1) & static_cast<int>(NUM_ELEMS_MASK); }

	/// Get an iterator over the elements of this set.
	Iterator getIterator() const;

	/// Return the iterator associated with key or the end iterator if not present.
	inline Iterator findElement( T elem ) const;

	/// Get the element at iterator i.
	T getElement( Iterator i ) const;

	/// Get the next iterator after i.
	Iterator getNext( Iterator i ) const;

	/// Return if the iterator has reached the end.
	hkBool32 isValid( Iterator i ) const;

	/// clear the set
	void clear();

	/// Reserve space for at least numElements;
	void reserve( int numElements );

	HK_FORCE_INLINE int getHashMod() const { return m_elem.getSize() - 1; }

protected:
	hkResult resizeTable(int newcap);

	// Internal flags, set in constructor.
	enum
	{
		NUM_ELEMS_MASK = int(0x7FFFFFFF),
		DONT_DEALLOCATE_FLAG = int(0x80000000), // Indicates that the storage is not the array's to delete
		s_minimumCapacity = 8
	};

	hkArray<T, Allocator> m_elem;
	int m_numElems; // high bits are flags

};

class hkSetUint32 : public hkSet<hkUint32>
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_MAP, hkSetUint32);
	HK_FORCE_INLINE hkSetUint32() {}
	HK_FORCE_INLINE hkSetUint32(hkFinishLoadedObjectFlag flag) : hkSet<hkUint32>(flag) {}
};

#include <Common/Base/Container/Set/hkSet.inl>

#endif // HKBASE_HKPOINTERMAP_H

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
