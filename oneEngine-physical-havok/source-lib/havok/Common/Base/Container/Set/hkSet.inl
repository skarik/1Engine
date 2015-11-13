/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename T, typename Allocator, typename OPS >
inline typename hkSet<T,Allocator,OPS>::Iterator hkSet<T,Allocator,OPS>::getIterator() const
{
	int i;
	const int hashMod = getHashMod();
	for( i = 0; i <= hashMod; ++i )
	{
		if( OPS::isValid( m_elem[i]  ) )
		{
			break;
		}
	}

	return HK_MAP_INDEX_TO_ITERATOR(i);
}

template <typename T, typename Allocator, typename OPS >
void hkSet<T,Allocator,OPS>::remove( Iterator it )
{
	int m_hashMod = getHashMod();

	HK_ASSERT(0x5a6d564c, isValid(it));
	HK_ASSERT(0x5a6d564d, getSize() > 0);
	unsigned i = HK_MAP_ITERATOR_TO_INDEX(it);

	// remove it
	--m_numElems;
	OPS::invalidate( m_elem[i] );

	// find lowest element of this unbroken run
	unsigned lo = ( i + m_hashMod ) & m_hashMod;
	while( OPS::isValid( m_elem[lo] ) )
	{
		lo = ( lo + m_hashMod ) & m_hashMod;
	}
	lo = ( lo + 1 ) & m_hashMod;

	// the slot which has become empty
	unsigned empty = i;

	// shift up, closing any gaps we find
	for(i = (i + 1) & m_hashMod;
		OPS::isValid( m_elem[i] ); // end of run
		i = (i + 1) & m_hashMod )
	{
		unsigned hash = OPS::hash( m_elem[i] , m_hashMod );

		// Three cases to consider here. 
		// a) The normal case where lo <= empty < i.
		// b) The case where i has wrapped around.
		// c) The case where both i and empty have wrapped around.
		// The initial case will be a. (Or b if the last slot is freed).
		// and may progress to b, and finally c.
		// The algorithm will terminate before 'i' reaches 'lo'
		// otherwise the table would have no free slots.

		// 'normal'      'i wrapped'   'i and empty wrapped'
		// ===== lo      ===== i       ===== empty
		// ===== empty   ===== lo      ===== i 
		// ===== i       ===== empty   ===== lo     


		if( ( i >= lo ) && ( hash > empty ) )
		{
			continue;					
		}
		else if( ( i < empty ) && ( hash > empty || hash <= i ) )
		{
			continue;
		}
		else if( /*i > empty && */ ( hash > empty && hash < lo ) )
		{
			continue;
		}
		HK_ASSERT(0x45e3d455,  i != empty ); // by design
		HK_ASSERT(0x5ef0d6c0,  i != lo ); // table became full?!

		// copy up
		m_elem[empty] = m_elem[i];
		// source slot is now free
		OPS::invalidate( m_elem[i] );
		empty = i;
	}
}

template <typename T, typename Allocator, typename OPS >
typename hkSet<T,Allocator,OPS>::Iterator hkSet<T,Allocator,OPS>::findElement( T key ) const
{
	const int m_hashMod = getHashMod();
	if( m_hashMod > 0 )
	{
		for( unsigned i = OPS::hash(key, m_hashMod);
			OPS::isValid( m_elem[i] );	
			i = (i+1) & m_hashMod)
		{
			if( OPS::equal( m_elem[i], key ) )
			{
				return HK_MAP_INDEX_TO_ITERATOR(i); // found
			}
		}
	}
	return HK_MAP_INDEX_TO_ITERATOR(m_hashMod+1); // not found
}

template <typename T, typename Allocator, typename OPS >
hkResult hkSet<T,Allocator,OPS>::remove( T key )
{
	Iterator it = findElement(key);
	if( isValid(it) )
	{
		remove(it);
		return HK_SUCCESS;
	}
	return HK_FAILURE;	
}

template <typename T, typename Allocator, typename OPS >
inline hkBool32 hkSet<T,Allocator,OPS>::isValid( Iterator it ) const
{
	const int m_hashMod = getHashMod();
	// range [0, hashMod] is valid
	// hashMod+1 invalid
	// anything else is bad input
	int i = HK_MAP_ITERATOR_TO_INDEX(it);
	HK_ASSERT(0x7f305156, i>=0 && i<=m_hashMod+1);
	return i <= m_hashMod;
}

template <typename T, typename Allocator, typename OPS >
inline typename hkSet<T,Allocator,OPS>::Iterator hkSet<T,Allocator,OPS>::getNext( Iterator it ) const
{
	const int m_hashMod = getHashMod();
	int i = HK_MAP_ITERATOR_TO_INDEX(it);
	HK_ASSERT(0x7f305156, i>=0 && i<=m_hashMod);
	for( i += 1; i <= m_hashMod; ++i )
	{
		if( OPS::isValid( m_elem[i] ) )
		{
			break;
		}
	}
	return HK_MAP_INDEX_TO_ITERATOR(i);
}

template <typename T, typename Allocator, typename OPS >
inline T hkSet<T,Allocator,OPS>::getElement(Iterator it) const
{
	int i = HK_MAP_ITERATOR_TO_INDEX(it);
	HK_ASSERT(0x7f305156, i>=0 && i<=getHashMod());
	return m_elem[i];
}

template <typename T, typename Allocator, typename OPS >
hkResult hkSet<T,Allocator,OPS>::resizeTable(int newcap)
{
	int m_hashMod = getHashMod();

	newcap = hkMath::max2( newcap, (int) s_minimumCapacity );
	HK_ASSERT2(0x57c91b4a,  m_numElems < newcap, "table size is not big enough" );
	HK_ASSERT2(0x6c8f2576,  HK_NEXT_MULTIPLE_OF(2, newcap) == newcap, "table size should be a power of 2" );

	//hkBool32 dontDeallocate = m_numElems & static_cast<int>(DONT_DEALLOCATE_FLAG);
	int oldcap = m_hashMod+1;
	//Pair* oldelem = m_elem;
	//Pair* newelem = static_cast<Pair*>( alloc.blockAlloc( sizeof(Pair)*newcap ) ); // space for values too

	hkArray<T, Allocator> oldelem;
	oldelem.swap(m_elem);
	hkResult reserveRes = m_elem.reserveExactly(newcap);

	// Make sure alloc succeeded
	if ( HK_VERY_UNLIKELY((reserveRes != HK_SUCCESS)) )
	{
		return HK_FAILURE;
	}

	m_elem.setSizeUnchecked(newcap);

	//m_elem = newelem;

	for (int i = 0; i < newcap; i++) 
	{
		OPS::invalidate( m_elem[i] ); 
	} 
	m_numElems = 0;
	m_hashMod = newcap - 1;

	for( int i = 0; i < oldcap; ++i )
	{
		if( OPS::isValid( oldelem[i]) )
		{
			insert( oldelem[i] );
		}
	}

	return HK_SUCCESS;
}

template <typename T, typename Allocator, typename OPS >
hkBool32 hkSet<T,Allocator,OPS>::insert( T key )
{
	HK_ASSERT2(0x19291575, OPS::isValid(key), "pointer map keys must not be the empty value");
	// This is quite conservative. We could grow more
	// slowly at the cost of potentially longer searches.
	{
		const int m_hashMod = getHashMod();
		int numElems = m_numElems & static_cast<int>(NUM_ELEMS_MASK);
		if( (numElems + numElems) > m_hashMod )
		{
			resizeTable( m_hashMod + m_hashMod + 2 );
		}
	}

	const int m_hashMod = getHashMod();

	unsigned i;
	hkBool32 isNewKey = true;
	for( i = OPS::hash(key, m_hashMod);
		OPS::isValid(m_elem[i]);
		i = (i+1) & m_hashMod )
	{
		// find free slot
		if( OPS::equal( m_elem[i], key ) )
		{
			isNewKey = false;
			break;
		}
	}

	// dont increment m_numElems if overwriting.
	m_numElems += isNewKey;

	// insert key,value
	m_elem[i] = key;
	return isNewKey;
}

template <typename T, typename Allocator, typename OPS >
hkBool32 hkSet<T,Allocator,OPS>::tryInsert( T key, hkResult& res)
{
	HK_ASSERT2(0x19291575, OPS::isValid(key), "hkSet elements must not be the empty value");
	// This is quite conservative. We could grow more
	// slowly at the cost of potentially longer searches.
	{
		const int m_hashMod = getHashMod();
		int numElems = m_numElems & static_cast<int>(NUM_ELEMS_MASK);
		if( (numElems + numElems) > m_hashMod )
		{
			res = resizeTable( m_hashMod + m_hashMod + 2);
			if (res != HK_SUCCESS)
			{
				return false;
			}
		}
		else
		{
			res = HK_SUCCESS;
		}
	}

	return insert( key );
}

template <typename T, typename Allocator, typename OPS >
void hkSet<T,Allocator,OPS>::clear()
{
	const int m_hashMod = getHashMod();
	int capacity = m_hashMod+1;
	for (int i = 0; i < capacity; i++)
	{
		OPS::invalidate( m_elem[i] ); 
	} 
	m_numElems = 0 | (m_numElems & static_cast<int>(DONT_DEALLOCATE_FLAG));
}

template <typename T, typename Allocator, typename OPS >
void hkSet<T,Allocator,OPS>::reserve( int numElements )
{
	// Make sure that the actual table size is not going to be less than twice the current number of elements
	HK_ASSERT(0x4d0c5314, numElements >= 0 && (m_numElems & static_cast<int>(NUM_ELEMS_MASK)) * 2 <= numElements * 3 );
	int minCap = numElements * 2;
	int cap = s_minimumCapacity;
	while (cap < minCap) { cap *= 2; }

	resizeTable( cap );
}

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
