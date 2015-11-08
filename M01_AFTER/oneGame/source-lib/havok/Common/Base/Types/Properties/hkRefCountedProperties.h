/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2012 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_BASE_REF_COUNTED_PROPERTIES_H
#define HK_BASE_REF_COUNTED_PROPERTIES_H

	/// Implements a collection of properties uniquely identified by a key
class hkRefCountedProperties : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		HK_DECLARE_REFLECTION();

		typedef hkUint16 PropertyKey;

	public:

			/// Entry in the array of properties
		struct Entry
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkRefCountedProperties::Entry);
				HK_DECLARE_REFLECTION();

			public:

				enum
				{
					INVALID_PROPERTY_KEY	= (PropertyKey)-1,	///< Invalid property key
				};

			public:

					/// Constructor
				Entry()
				:	m_object(HK_NULL)
				,	m_key(INVALID_PROPERTY_KEY)
				,	m_flags(0)
				{}

					/// Serialization constructor
				Entry(class hkFinishLoadedObjectFlag flag)
				:	m_object(flag)
				{}

					/// Returns the key
				HK_FORCE_INLINE PropertyKey getKey() const			{ return m_key; }

					/// Sets the key
				HK_FORCE_INLINE void setKey(PropertyKey newKey)		{ m_key = newKey; }

					/// Returns the flags
				HK_FORCE_INLINE hkUint16 getFlags() const			{ return m_flags; }

					/// Sets the flags
				HK_FORCE_INLINE void setFlags(hkUint16 newFlags)	{ m_flags = newFlags; }

			public:

					/// The object
				hkRefPtr<hkReferencedObject> m_object;

					/// The key
				PropertyKey m_key;

					/// The flags
				hkUint16 m_flags;
		};

	public:
		
			/// Constructor
		hkRefCountedProperties();

			/// Serialization constructor
		hkRefCountedProperties(hkFinishLoadedObjectFlag flag);

			/// Destructor
		virtual ~hkRefCountedProperties();

	public:

			/// Adds a property to the collection. If a property is already installed for the given key, it will be replaced
		void addProperty(PropertyKey propertyKey, hkReferencedObject* propertyObject);

			/// Removes a property from the collection
		void removeProperty(PropertyKey propertyKey);

			/// Replaces the property at the given key with the given object
		void replaceProperty(PropertyKey propertyKey, hkReferencedObject* newPropertyObject);

			/// Locates and returns the property at the given key. If no property was found, it will return null.
		hkReferencedObject* getProperty(PropertyKey propertyKey) const;

	protected:

			/// The array of entries
		hkArray<Entry> m_entries;
};

#endif	//	HK_BASE_REF_COUNTED_PROPERTIES_H

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
